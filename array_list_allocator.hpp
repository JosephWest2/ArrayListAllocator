#pragma once

#include <absl/container/btree_set.h>

#include <functional>
#include <memory>
#include <optional>

#include "random.hpp"

namespace jw2 {

#define CreateArrayListAllocator(T) jw2::ArrayListAllocator<T, size_t, size_t, JW2_CONSTEXPR_RANDOM>()
#define CreateArrayListAllocator2(T, IndexType, GenerationType) \
    jw2::ArrayListAllocator<T, IndexType, GenerationType, JW2_CONSTEXPR_RANDOM>()
#define GetHandleType(allocator_instance) decltype(allocator_instance)::HandleType

template <typename T, typename Index = size_t, typename Generation = size_t, uint32_t Tag = 0>
class Handle {
  private:
    Index index;
    Generation generation;

    Handle(Index index, Generation generation) : index(index), generation(generation) {}

    template <typename T2, typename Index2, typename Generation2, uint32_t Tag2>
    friend class ArrayListAllocator;
};

template <typename T, typename Index = size_t, typename Generation = size_t, uint32_t Tag = 0>
class ArrayListAllocator {
  private:
    struct Node {
        T value;
        Generation generation;
    };

    constexpr static Index START = 2;
    constexpr static Index DELETED = 1;
    constexpr static Index INVALID = 0;

    constexpr static uint GROWTH_FACTOR = 2;
    constexpr static uint SHRINK_THRESHOLD = 3;
    constexpr static uint MIN_SHRINK_FACTOR = 6;

    constexpr static Index MIN_CAPCITY = 8;

    std::allocator<Node> _allocator{};
    Index _capacity{};
    Node* _data{};
    Index _last = 0;
    Generation _generation = START;
    absl::btree_set<Index> _free_list{};

  public:
    using HandleType = Handle<T, Index, Generation, Tag>;

    ArrayListAllocator(Index capacity = MIN_CAPCITY)
        : _capacity(capacity), _data(_allocator.allocate(static_cast<size_t>(capacity))) {}

    ~ArrayListAllocator() {
        auto it = _free_list.begin();
        for (size_t i = 0; i < _last; i++) {
            if (it != _free_list.end() && *it == i) {
                it++;
                continue;
            }
            _data[i].value.~T();
        }
        _allocator.deallocate(_data, _last + 1);
    }

    [[nodiscard]]
    HandleType insert(const T& value) {
        return insert(T{value});
    }

    [[nodiscard]]
    HandleType insert(T&& value) {
        if (!_free_list.empty()) {
            auto it = _free_list.begin();
            std::construct_at(&_data[*it], Node{std::forward<T>(value), _generation});
            _free_list.erase(it);
            return HandleType(*it, _generation);
        }
        _last++;
        if (_last == _capacity) {
            grow();
        }
        std::construct_at(&_data[_last], Node{std::forward<T>(value), _generation});
        return HandleType(_last, _generation);
    }

    // calls the destructor on the object if it exists and
    // returns whether or not the element was found
    [[nodiscard]]
    bool destruct(const HandleType& handle) {
        if (handle.index > _last) {
            return false;
        }

        auto& node = _data[handle.index];
        if (node.generation != handle.generation) {
            return false;
        }

        node.generation = DELETED;

        _generation++;

        node.value.~T();

        if (handle.index == _last) {
            _last--;
            auto it = _free_list.rbegin();
            while (it != _free_list.rend() && *it == _last) {
                _last--;
                _free_list.erase(*it);
                it = _free_list.rbegin();
            }

            shrink_if_needed();
            return true;
        }

        _free_list.insert(handle.index);
        return true;
    }

    [[nodiscard]]
    std::optional<std::reference_wrapper<T>> operator[](const HandleType& handle) const {
        if (!is_valid_handle(handle)) {
            return std::nullopt;
        }
        return std::ref(_data[handle.index].value);
    }

    [[nodiscard]]
    bool is_valid_handle(const HandleType& handle) const {
        return handle.index <= _last && _data[handle.index].generation == handle.generation;
    }

    [[nodiscard]]
    size_t item_count() const {
        return _last - _free_list.size();
    }

    [[nodiscard]]
    size_t capacity() const {
        return _capacity;
    }

  private:
    void grow() {
        auto new_capacity = _capacity * GROWTH_FACTOR;
        auto new_data = _allocator.allocate(static_cast<size_t>(new_capacity));
        auto it = _free_list.begin();
        for (size_t i = 0; i < _last; i++) {
            if (it != _free_list.end() && *it == i) {
                it++;
                continue;
            }
            new_data[i] = std::move(_data[i]);
            _data[i].value.~T();
        }
        _allocator.deallocate(_data, _capacity);
        _data = new_data;
        _capacity = new_capacity;
    }

    void shrink_if_needed() {
        if (item_count() <= _capacity / SHRINK_THRESHOLD) {
            shrink();
        }
    }

    void shrink() {
        if (_capacity - _last < _capacity / MIN_SHRINK_FACTOR) {
            return;
        }
        auto new_capacity = std::max(_last * GROWTH_FACTOR - _free_list.size(), MIN_CAPCITY);
        auto new_data = _allocator.allocate(static_cast<size_t>(new_capacity));
        auto it = _free_list.begin();
        for (size_t i = 0; i < _last; i++) {
            if (it != _free_list.end() && *it == i) {
                it++;
                continue;
            }
            new_data[i] = std::move(_data[i]);
            _data[i].value.~T();
        }
        _allocator.deallocate(_data, _capacity);
        _data = new_data;
        _capacity = new_capacity;
    }
};

};  // namespace jw2
