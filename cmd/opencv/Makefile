TARGET1 = fltest_Sobel
#CXX = aarch64-poky-linux-g++ --sysroot=/work/lzb/imx8m/sdk/sysroots/aarch64-poky-linux
CFLAGS +=  -lopencv_core -lopencv_objdetect -lopencv_highgui -lopencv_videoio -lopencv_imgproc -lOpenCL -lpthread -lrt

all:
	@$(CXX)  Sobel.cpp -o $(TARGET1) $(CFLAGS) 
	
clean:
	rm -rf $(TARGET1) 
