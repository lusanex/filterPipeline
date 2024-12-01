#!/bin/bash

# Variables
STREAM_EXECUTABLE="./stream"           # Path to the compiled stream program
INPUT_VIDEO="./output.mp4"         # Input video file


# Check if input video exists
if [ ! -f "$INPUT_VIDEO" ]; then
    echo "$INPUT_VIDEO: No such file or directory"
    exit 1
fi


# Get video metadata using FFmpeg
WIDTH=$(ffprobe -v error -select_streams v:0 -show_entries stream=width -of csv=p=0 "$INPUT_VIDEO")
HEIGHT=$(ffprobe -v error -select_streams v:0 -show_entries stream=height -of csv=p=0 "$INPUT_VIDEO")
PIX_FMT=rgba  # Converting to RGBA format
FPS=$(ffprobe -v error -select_streams v:0 -show_entries stream=r_frame_rate -of csv=p=0 "$INPUT_VIDEO")
DURATION=$(ffprobe -v error -show_entries format=duration -of csv=p=0 "$INPUT_VIDEO")

# Convert frame rate to a single value
FPS=$(echo "$FPS" | bc)

# Prepare header
HEADER=$(cat <<EOF
VIDEO_STREAM_HEADER
WIDTH:$WIDTH
HEIGHT:$HEIGHT
PIX_FMT:$PIX_FMT
FPS:$FPS
DURATION:$DURATION
HEADER_END
EOF
)

# Compile the stream program
echo "Compiling stream.cpp..."
g++ stream.cpp -o "$STREAM_EXECUTABLE" || { echo "Compilation failed"; exit 1; }


# Process the video and pipe it to ffplay for live playback
echo "Processing video..."

( echo "$HEADER"
ffmpeg -i "$INPUT_VIDEO" -f rawvideo -pix_fmt "$PIX_FMT" - 2>/dev/null) | "$STREAM_EXECUTABLE" | ffplay -f rawvideo -pixel_format rgba -video_size "${WIDTH}x${HEIGHT}" -



