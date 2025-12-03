#include "array_list_allocator.hpp"

#include <optional>
#include <print>


int main() {

    auto list = CreateArrayListAllocator(int);
    auto h1 = list.insert(1);
    auto h2 = list.insert(2);
    using HandleType = GetHandleType(list);

    std::vector<HandleType> handles{};

    for (size_t i = 0; i < 1000; i++) {
        handles.push_back(list.insert(i));
    }
    assert(list.item_count() == 1002);

    assert(list.is_valid_handle(h1));
    assert(list.is_valid_handle(h2));

    auto v = list[h1];
    assert(v.value().get() == 1);
    auto v2 = list[h2];
    assert(v2.value().get() == 2);

    assert(list[h1].value() == 1);
    auto list2 = CreateArrayListAllocator(int);

    assert(list.destruct(h2));

    assert(!list.is_valid_handle(h2));
    assert(list.item_count() == 1001);

    list[h1].value().get() = 55;
    assert(list[h1].value().get() == 55);


    std::println("capacity: {}", list.capacity());

    auto hc = handles[55];

    for (auto& handle : handles) {
        bool _ = list.destruct(handle);
    }
    std::println("capacity: {}", list.capacity());

    for (size_t i = 0; i < 1000; i++) {
        handles.push_back(list.insert(i));
    }
    assert(list.item_count() == 1001);
    assert(!list.is_valid_handle(hc));

    std::println("tests passed");
}
