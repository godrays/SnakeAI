# Project Build Instructions

Follow these steps to build the project and make it deployment-ready.

Currently, it supports only macOS and has been tested on Sonoma.

---

### Step 1: Build Externals

This step will build external libraries.

```bash
cd Externals
./build_all.sh
```

### Step 2: Build Targets

This step will build all binaries and deploy into a specific folder.

```bash
./build.sh release product-rel
```

After a successful build, all target binaries will be deployed into the product-rel folder.

Note: Run the build.sh file without parameters to see all options.

# How to Train and Play SnakeAI

Follow the following steps to train and play model.

### Step 1: Train

```bash
cd product-rel
./SnakeAIApp ga train --modelfile=snakeai.mdl --maxGen=500
```

### Step 2: Play

```bash
./SnakeAIApp ga play --modelfile=snakeai.mdl
```

Note: Run SnakeAIApp without any parameters to see all command line options.
