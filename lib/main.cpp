#include <ffi.h>
#include <vector>
#include <cstdlib>


int main() {
    ffi_cif Cif;
    std::vector<ffi_type*> Args(1);
    Args[0] = &ffi_type_uint64;

    ffi_status res = ffi_prep_cif(&Cif, FFI_DEFAULT_ABI, 1, &ffi_type_pointer, Args.data());
    void* ptr;
    std::vector<void*> Rargs(1);
    size_t Size = 0xffdd11;
    Rargs[0] = &Size;

    void *fn_ptr = malloc;
    void (*fn)() = reinterpret_cast<void(*)()>(fn_ptr);
    ffi_call(&Cif, fn, &ptr, Rargs.data());

    printf("%x", ptr);
}