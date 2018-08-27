# FastFunction

FastFunction is a function wrapper that is designed to be similar to std::function, but with no runtime memory allocation and minimal call indirection overhead.

## Storage/copying efficiency:
There will be no dynamic heap allocation at runtime because the wrapper stores the target callable in a preallocated storage.
The size of this storage is determined at construction.

## Invocation efficiency:
Similar as libstd++ and boost; one pointer indirection between FastFunction and function object target.
