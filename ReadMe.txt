This C++ console application makes use of Microsoft Media Foundation to transcode video (with any number of streams and any format supported by Windows). Hardware acceleration is enabled whenever possible. Encoding to H.264 is very fast!

The solution is meant to be build by Visual C++ 2015 and depends on 3FD v2.4+ (https://github.com/faburaya/3fd) installed in a location defined by an enviroment variable to be called "_3FD_HOME".

Windows 10 (at least build 14393) is recommended.

Usage:

 VideoTranscoder [/e:encoder] [/t:target_size_factor] input output

            encoder = h264 (default)
 target size factor = 0.5 (default)
Must provide input & output files!

      /e:, /encoder:   What encoder to use, always with the highest profile
                       made available by Microsoft Media Foundation, for better
                       compression - allowed: [(default = )h264, hevc]

          /t:, /tsf:   The target size of the output transcoded video, as a
                       fraction of the original size (when ommited, default =
                       0.5; min = 0.001; max = 1)

        input output   input & output files (expects 2 values)
