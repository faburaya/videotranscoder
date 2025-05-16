This C++ console application makes use of Microsoft Media Foundation to transcode video.
Hardware acceleration is enabled whenever possible. Encoding with HEVC is very fast!

The solution is meant to be build by Visual C++ 2022.
You have everything you need to build, including dependencies, in this repository.

Usage example:

 VideoTranscoder -i input.mp4 -o output.mp4 -e hevc -t 0.5

OPTIONS:
  -h,     --help              Print this help message and exit
  -i,     --input TEXT REQUIRED
                              Input video file
  -o,     --output TEXT REQUIRED
                              Output MP4 file
  -e,     --encoder TEXT:{hevc,h264} REQUIRED
                              Video encoder to use (from Microsoft Media Foundation)
  -t,     --tsf FLOAT:FLOAT in [0 - 1] REQUIRED
                              Target size factor
