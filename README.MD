# Project Build Instructions

Follow the following steps to build the project and make it deployment ready.

Note: Currently support only macOS and tested on Sonoma.

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

After the successful build, all target binaries will be deployed into the product-rel folder.

Note: Run the build.sh file without parameters to see all options.
