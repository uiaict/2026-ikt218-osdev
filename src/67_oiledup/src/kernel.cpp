extern "C" int kernel_main(void);

int kernel_main(){
    
    //Allocation test
    int* alloc = new int(12);
    delete alloc;
    
    // An infinite loop at the end of the kernel is good practice
    // so it doesn't accidentally return into nothingness!
    while(1) {} 
    return 0;
}