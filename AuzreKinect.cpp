#pragma comment(lib, "k4a.lib")
#include <k4a/k4a.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>

int main()
{
    uint32_t count = k4a_device_get_installed_count();
    if (count == 0)
    {
        printf("No k4a devices attached!\n");
        return 1;
    }

    // Open the first plugged in Kinect device
    k4a_device_t device = NULL;
    if (K4A_FAILED(k4a_device_open(K4A_DEVICE_DEFAULT, &device)))
    {
        printf("Failed to open k4a device!\n");
        return 1;
    }

    // Get the size of the serial number
    size_t serial_size = 0;
    k4a_device_get_serialnum(device, NULL, &serial_size);

    // Allocate memory for the serial, then acquire it
    char* serial = (char*)(malloc(serial_size));
    k4a_device_get_serialnum(device, serial, &serial_size);
    printf("Opened device: %s\n", serial);
    free(serial);

    // Configure a stream of 4096x3072 BRGA color data at 15 frames per second
    k4a_device_configuration_t config = K4A_DEVICE_CONFIG_INIT_DISABLE_ALL;	//
    config.color_format = K4A_IMAGE_FORMAT_COLOR_BGRA32;
    config.color_resolution = K4A_COLOR_RESOLUTION_1080P;
    config.depth_mode = K4A_DEPTH_MODE_NFOV_UNBINNED;
    config.camera_fps = K4A_FRAMES_PER_SECOND_30;

    // Start the camera with the given configuration
    if (K4A_FAILED(k4a_device_start_cameras(device, &config)))
    {
        printf("Failed to start cameras!\n");
        k4a_device_close(device);
        return 1;
    }

    // Camera capture and application specific code would go here
    k4a_capture_t capture;//declare the capture
    k4a_wait_result_t get_capture_result = k4a_device_get_capture(device, &capture, K4A_WAIT_INFINITE);
    //k4a_device_get_capture(device, &capture, 1);//get a capture

    // Access the depth16 image
    k4a_image_t Depthimage = k4a_capture_get_depth_image(capture);
    if (Depthimage != NULL)
    {
        printf(" | Depth16 res:%4dx%4d stride:%5d\n",
            k4a_image_get_height_pixels(Depthimage),
            k4a_image_get_width_pixels(Depthimage),
            k4a_image_get_stride_bytes(Depthimage));

        // Release the image
        k4a_image_release(Depthimage);
    }

    // Access the RGB image
    k4a_image_t rgbImage = k4a_capture_get_color_image(capture);
    if (rgbImage != NULL)
    {
        printf(" | RGBA res:%4dx%4d \n",
            k4a_image_get_height_pixels(rgbImage),
            k4a_image_get_width_pixels(rgbImage));

        //get the buffer to futher manipulate with the data
        uint8_t* rgb = k4a_image_get_buffer(rgbImage);

        // Release the image
        k4a_image_release(rgbImage);
    }

    // Access the IR image
    k4a_image_t irImage = k4a_capture_get_ir_image(capture);
    if (irImage != NULL)
    {
        printf(" | IR res:%4dx%4d stride:%5d\n",
            k4a_image_get_height_pixels(irImage),
            k4a_image_get_width_pixels(irImage),
            k4a_image_get_stride_bytes(irImage));

        //get the buffer to futher manipulate with the data
        uint8_t* ir = k4a_image_get_buffer(irImage);

        // Release the image
        k4a_image_release(irImage);
    }

    // Release the capture
    k4a_capture_release(capture);

    // Shut down the camera when finished with application logic
    k4a_device_stop_cameras(device);
    k4a_device_close(device);

    return 0;
}

