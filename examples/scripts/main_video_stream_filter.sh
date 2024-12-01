#!/bin/bash

# Determine the base name relative to the script's location
BASE_NAME=$(dirname "$(readlink -f "$0")")
echo "$BASE_NAME"

# Variables
EXECUTABLE_NAME="${BASE_NAME}/../bin/stream" # Path to the compiled stream program
SOURCE_NAME=mainStreamFilter.cpp
ASSETS_PATH="${BASE_NAME}/../../assets"
INPUT_VIDEO="${ASSETS_PATH}/rolling_cat_480p.mp4" # Input video file
BIN_PATH="${BASE_NAME}/../bin"
OUT_PATH="${BASE_NAME}/../out"

# Check if input video exists
if [ ! -f "$INPUT_VIDEO" ]; then
    echo "$INPUT_VIDEO: No such file or directory"
    exit 1
fi

# Create necessary directories
mkdir -p "${BIN_PATH}"
mkdir -p "${OUT_PATH}"

# Compile the stream program
echo "Compiling stream example..."
g++ "${SOURCE_NAME}" -o "${EXECUTABLE_NAME}" || { echo "Compilation failed"; exit 1; }

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

# Process the video and pipe it to ffplay for live playback
echo "Processing video..."

( echo "$HEADER"
ffmpeg -i "$INPUT_VIDEO" -f rawvideo -pix_fmt "$PIX_FMT" - 2>/dev/null) | "${EXECUTABLE_NAME}" | ffplay -f rawvideo -pixel_format ${PIX_FMT} -video_size "${WIDTH}x${HEIGHT}" -

