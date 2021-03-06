# Data storage system
To compile the library and checker run **make**. Checker is called **test_storage**
and shows implementation of common functions from the library.

Include **storage.h** in your file to use the library

Below are descriptions of available functions

For system initialization the function is passed,
which destroys elements with respect to their type. The function's prototype
is supposed to be like "int ext_delete_func(int type, void *struc)" and
return 0 if it succeeded and 1 if it failed. Second argument is a
function that checks uniqueness of the element. It must return 0 if elements
are identical and any other integer if they are not. It should be prototyped
like "int ext_uniq_func(int type, void *struc1, void *struc2)". System_init
function returns 0 if the system was successfully initialized by this function
call. If initialization failed or the system was already initialised,
the function returns 1.
```
system_init(&ext_delete_func, &ext_uniq_func);
```

To add new element there should be passed the structure, its type and
macro: ADD_UNIQUE or ADD_ELEM to either add an element only if it is unique or
add element irregardless of duplicates.
Type should be a positive integer. If type is zero it is considered that type is
unknown and function returns fail. If element is not added, the return value is 1,
otherwise it is 0.
```
add_element(struc, struc->type, ADD_UNIQUE);
```

To delete a specific element there should be passed a structure
and its type. If one or more elements were found and removed the return value
is 0. If the element wasn't found the return value is 1.
```
delete_element(struc, struc->type);
```

To perform an action to all elements of the same type there should be passed
type value and function, that will be applied to elements.
iterate_elements function does not return any value.
```
iterate_elements(struc->type, &ext_iter_func);
```

To deinitialize the system use system_destroy function. It does not need any
arguments. It frees all structures that were previously passed as arguments
using external delete function. If on any stage object is not properly destroyed
the function continues to free the memory where it is possible, but returns
value 1 which stays for failure. If everything was successfully removed the
function returns 0.
```
system_destroy();
```
