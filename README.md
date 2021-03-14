# stb_wrapper
A c++ wrapper for stb image library.

[stb_image](https://github.com/nothings/stb)是一个简单易用的header-only图像库，使用时只需包含.h文件即可。

使用参考：https://glumes.com/post/android/stb-image-introduce/

> STB Image库的编解码速度并不快，建议测试验证时使用，而不要在产品中直接使用。在产品中应该使用libjpeg-turbo、libpng库，它会使用多媒体加速指令（例如，ARM平台的NEON指令）。