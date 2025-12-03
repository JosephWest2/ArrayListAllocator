# ArrayListAllocator
A data structure that constructs and destructs passed objects in a single array buffer, and returns an opaque handle tied exclusively to the allocator instance that can be used to retrieve a reference to the object. The handle contains simply an index and a generation that is used to ensure that the the object in memory is not a new object constructed in the old's place. This is largely inspired by (Handles Are The Better Pointers)[https://floooh.github.io/2018/06/17/handles-vs-pointers.html]

```cpp

// Allocator's should be created with the macro as this ensures the allocator get's a unique Template tag, used to tie handles exclusively to this instance
auto allocator = CreateArrayListAllocator(int);

// handle type can be retrieved with the macro
GetHandleType(allocator);
// which just expands to
decltype(allocator)::HandleType;

// Usage
auto h1 = allocator.insert(1);

int val = allocator[h1].value().get();

allocator.destruct(h1);

// h1 is now invalid, and using it will result in getting a nullopt
assert(allocator[h1] == std::nullopt);


```
