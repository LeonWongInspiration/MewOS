void io_hlt(void);

void MewOSMain(){
	while (true){
		io_hlt();
	}
}
