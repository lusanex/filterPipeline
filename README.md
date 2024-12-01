# filterPipeline
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
  - `enter`: Called at the start of the calculator's lifecycle (for initialization).
  - `process`: Performs the main processing task.
  - `close`: Called at the end of the calculator's lifecycle (for cleanup).
- Derived classes implement specific functionality by overriding these methods.

### 2. Image

**Purpose**: Represents an image with raw pixel data.

**Key Features**:
- Holds image data in **RGBA format**.
- Provides metadata like width, height, and pixel format.
- Utility methods to manipulate image data (e.g., pixel-by-pixel operations).

### 3. Packet

**Purpose**: A general-purpose container for passing data between calculators.

**Key Features**:
- Contains a timestamp for synchronization.
- Holds a `PacketHolder`, which wraps a template type (`T`) for polymorphic data handling.
- Ensures flexibility for different types of data.

### 4. PacketHolder

**Purpose**: Base class for `Packet`, enabling polymorphism.

**Key Features**:
- Templates allow `PacketHolder` to store and manipulate specific data types (`T`).

### 5. Port

**Purpose**: Acts as a queue for `Packet`s.

**Key Features**:
- Supports **write** and **read** operations for data exchange.
- Ensures thread-safe and organized communication between calculators.

### 6. CalculatorContext

**Purpose**: Holds the execution context for each calculator.

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
- Implements **time slicing** to allow each calculator a defined period to process data.
- Computes **delta time** to measure the time elapsed between frames.
- Enforces a frame rate limit using `FRAME_RATE_MS`.
- Provides callbacks:
  - **Input Callback**: Supplies input data to the pipeline.
  - **Output Callback**: Handles processed data from the pipeline.

---

## Framework Workflow

### Header Parsing:
- Video or image metadata is parsed and validated at the beginning.
- This information (e.g., dimensions, pixel format) is passed to the framework.

### Calculator Registration:
- Each calculator is derived from `CalculatorBase`.
- Calculators define their input/output ports and specific processing logic.
- The `Scheduler` registers calculators and retrieves their contexts.

### Data Flow:
- Input data flows into the pipeline through the **Input Callback**.
- The `Scheduler` passes data between calculators using their `process` methods.
- Processed data is output via the **Output Callback**.

### Time Management:
- The `Scheduler` calculates delta time and ensures that each calculator gets a fair share of processing time.
- Frame rate is enforced to maintain smooth operation.

---

## Example Usage

### Setting Up the Pipeline

#### Create Calculators:
- Implement custom calculators by deriving from `CalculatorBase` and overriding `enter`, `process`, and `close`.

#### Register Calculators:
- Add calculators to the `Scheduler` and configure their contexts with side packets and ports.

#### Configure Callbacks:
- Provide input and output callbacks to handle data flow.

#### Run the Scheduler:
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


```cpp
Scheduler scheduler;

GrayscaleCalculator grayscaleCalculator;
scheduler.registerCalculator(&grayscaleCalculator);

scheduler.registerInputCallback([](void* ctx) -> Packet {
    // Read next input data 
    return Packet(Image(...));
}, nullptr);

scheduler.registerOutputCallback([](const Packet& packet) {
    // Handle processed output data
});
scheduler.connectCalculators();
scheduler.run();

```


## Atributions
Video by Jake Heinemann from Pexels: https://www.pexels.com/video/cute-cat-1481903/
