# Filter Pipeline Framework

## Overview

The **Filter Pipeline Framework** is a modular system designed to process video or image streams using a sequence of calculators. Each calculator performs specific tasks on the data, such as grayscale conversion, dithering, pixelation, or applying overlays. The framework is built around a central **Scheduler** that manages calculators and their associated contexts, ensuring smooth execution and data flow.

This document provides a detailed explanation of the framework's components and their roles.

---

## Core Components

### 1. CalculatorBase

**Purpose**: Abstract base class for all calculators.

**Key Features**:
- Defines a **lifecycle** with three methods:
  - `enter`: Called at the start of the calculator's lifecycle (executed for every time slice allocated to the calculator).
  - `process`: Performs the main processing task (called for at least one frame of data).
  - `close`: Called at the end of the calculator's lifecycle (for cleanup).
- Derived classes implement specific functionality by overriding these methods.

### 2. Image

**Purpose**: Represents an image with raw pixel data.

**Key Features**:
- Stores image data in **RGBA** or **RGB format**.
- Provides metadata such as width, height, and pixel format.
- Includes utility methods for manipulating image data (e.g., pixel-by-pixel operations).

### 3. Packet

**Purpose**: A general-purpose container for passing data between calculators.

**Key Features**:
- Includes a timestamp for synchronization.
- Encapsulates a `PacketHolder`, which wraps a template type (`T`) for polymorphic data handling.
- Allows flexible handling of various data types.

### 4. PacketHolder

**Purpose**: Base class for `Packet`, enabling polymorphism.

**Key Features**:
- Uses templates to store and manipulate specific data types (`T`).

### 5. Port

**Purpose**: Acts as a queue for `Packet`s.

**Key Features**:
- Supports **write** and **read** operations for data exchange.
- Provides a thread-safe mechanism for communication between calculators.

### 6. CalculatorContext

**Purpose**: Manages the execution context for each calculator.

**Key Features**:
- Contains:
  - **Input Ports**: A map of ports for receiving data.
  - **Output Ports**: A map of ports for sending data.
  - **Side Packets**: A shared map of configuration parameters or constants accessible by the calculator.
- Each calculator creates and manages its own context during registration.

### 7. Scheduler

**Purpose**: Orchestrates the execution of calculators in the pipeline.

**Key Features**:
- Registers calculators and retrieves their contexts.
- Manages a sequence of calculators, ensuring they execute in order.
- Implements **time slicing**, giving each calculator a defined period to process data.
- Computes **delta time** to measure the time elapsed between frames.
- Enforces a frame rate limit using `FRAME_RATE_MS`.
- Provides callbacks:
  - **Input Callback**: Supplies input data to the pipeline.
  - **Output Callback**: Handles processed data from the pipeline.

### 8. ImageUtils

**Purpose**: Helper class for working with BMP images.

**Key Features**:
- Includes methods for loading BMP images and converting them into the `Image` class.
- Provides functionality to save `Image` objects back to BMP format.

---

## Framework Workflow


### Calculator Registration
- Each calculator is derived from `CalculatorBase`.
- Calculators define their input/output ports and specific processing logic.
- The `Scheduler` registers calculators and retrieves their contexts.

### Data Flow
- Input data is fed into the pipeline through the **Input Callback**.
- The `Scheduler` passes data between calculators using their `process` methods.
- Processed data is output via the **Output Callback**.

### Time Management
- The `Scheduler` calculates delta time to measure elapsed time between frames.
- Ensures fair processing time for each calculator by enforcing a frame rate.

---

## Example Usage

### Setting Up the Pipeline

#### Create Calculators
- Implement custom calculators by deriving from `CalculatorBase` and overriding `enter`, `process`, and `close`.

#### Register Calculators
- Add calculators to the `Scheduler` and configure their contexts with side packets and ports.

#### Configure Callbacks
- Provide input and output callbacks to handle data flow.

#### Run the Scheduler
- The `Scheduler` processes frames in real-time, invoking calculators sequentially.

---

## Example Code

### Calculator Implementation
```cpp
class GrayscaleCalculator : public CalculatorBase {
public:
    GrayscaleCalculator() : CalculatorBase("GrayscaleCalculator") {}

    void enter(CalculatorContext* cc, float delta) override {}
    void process(CalculatorContext* cc, float delta) override {
        Port& inputPort = cc->getInputPort("ImageInput");
        if (inputPort.size() == 0) return;

        Packet inputPacket = inputPort.read();
        Image image = inputPacket.get<Image>();
        // Convert image to grayscale...
        cc->getOutputPort("ImageOutput").write(Packet(std::move(image)));
    }
    void close(CalculatorContext* cc, float delta) override {}
};

    Scheduler scheduler;

    GrayscaleCalculator grayscaleCalculator;
    scheduler.registerCalculator(&grayscaleCalculator);

    scheduler.registerInputCallback([](void* ctx) -> Packet {
        // Callback to read input data
        return Packet(Image(...));
    }, nullptr);

    scheduler.registerOutputCallback([](const Packet& packet) {
        // Handle processed output data
    });
    scheduler.connectCalculators();
    scheduler.run();


```
## Example Usage

A complete example demonstrating how to decode video frames from an MP4 file and pass them as a stream of bytes is available in the `examples` folder.

---

### Running the Example

1. Navigate to the `examples` folder.
2. Run the provided script:

   ```sh
   ./scripts/main_video_stream_filter.sh
   ```

3. This script:
   - Compiles the main entry point `mainStreamFilter.cpp`.
   - Uses the calculators in `scripts/calculators` to apply filters.
   - Streams the video frames with FFmpeg and processes them through the framework.

---

### Header Parsing
- Video or image metadata is parsed in the main entry and validated at the beginning.
- The header contains information like dimensions and pixel format, which are used to set up the framework for processing incoming data.

### Header Format

The video header is a simple string in the following format:

```sh
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
```

This header provides essential information such as the frame dimensions and pixel format.

---

## Preview: Before and After

Below are examples of the video frames before and after processing through the pipeline:

### Original Frame (Before Processing)
![Original Frame](assets/original_frame.png)

### Processed Frame (After Applying Filters)
![Processed Frame](assets/processed_frame.png)

---



---

## Attributions

Video by Jake Heinemann from Pexels: [https://www.pexels.com/video/cute-cat-1481903/](https://www.pexels.com/video/cute-cat-1481903/)


