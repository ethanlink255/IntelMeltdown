int main() {
    char *kernel_data_addr = (char*)0xfb9683cd;
    char kernel_data = *kernel_data_addr;
    printf("I have reached here.\n");
    return 0;
}