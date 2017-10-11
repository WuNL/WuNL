# WuNL
ffmpeg+opengl

boost asio接收rtp流，过滤后送到解码模块解码，解码模块将解完的YUV压入队列，显示模块读取队列进行显示

解码使用Nvidia的h264_cuvid解码器
解码完成后opengl使用shadel完成nv12到RGB的转换,尽量不使用CPU

