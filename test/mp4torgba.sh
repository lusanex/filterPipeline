
#!/bin/bash

# Variables
INPUT_VIDEO="../assets/output.mp4"         # Input video file


# Check if input video exists
if [ ! -f "$INPUT_VIDEO" ]; then
    echo "$INPUT_VIDEO: No such file or directory"
    exit 1
fi


# Get video metadata using FFmpeg
WIDTH=$(ffprobe -v error -select_streams v:0 -show_entries stream=width -of csv=p=0 "$INPUT_VIDEO")
HEIGHT=$(ffprobe -v error -select_streams v:0 -show_entries stream=height -of csv=p=0 "$INPUT_VIDEO")
PIX_FMT="rgba"  # Converting to RGBA format
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

#echo  "$PIX_FMT"
# Output the header
#echo "Sending header to stream..."

echo "$HEADER" 

# Process the video and pipe it to ffplay for live playback
echo "Processing video..."
ffmpeg -i "$INPUT_VIDEO" -f rawvideo -pix_fmt "$PIX_FMT" - 2>/dev/null 
#ffplay -f rawvideo -pixel_format "$PIX_FMT" -video_size "${WIDTH}x${HEIGHT}" -
