extern "C" int kernel_main(void);

int kernel_main(){
    
    // An infinite loop at the end of the kernel is good practice
    // so it doesn't accidentally return into nothingness!
    while(1) {} 
    return 0;
}