# Remote Control using Gmail

## Prerequisites
* **C++20**
* **CMake 3.24** or later

**Libraries for Client:**
* curl
* OpenSSL
* nlohmann/json
* wxWidgets

**Libraries for Server:**
* GDI+
* OpenCV

## Installation
First, clone this repo:
```bash 
git clone https://github.com/ThuongHong/RemoteControl.git
```
# Client
## curl
* Clone the CURL repository:
```bash
git clone https://github.com/curl/curl.git 
``` 
* In client folder, create new folder `lib`.
* Open CMake GUI
    * **"Where is the source code"**: Select the cloned ```curl``` folder.
    * **"Where to build the binaries"**: Select the ```/lib/curl``` dỉrectory.
    * Click **Configure**.
    * After the first configuration, use the search box at the top to look for the following option:
        * ```BUILD_SHARED_LIBS```: Uncheck to disable shared library building.
        * `CURL_USE_OPENSSL`: Check to enable OpenSSL support.
        * `CMAKE_INSTALL_PREFIX`: Set this to `/lib/curl/install`.
    * After setting the options, click **Configure again** if prompted to apply changes. Once configuration is complete and there are no errors, click **Generate** to create the build files.

* Click **Open Project**:
    * Select **Release** configuration.
    * Locate the **INSTALL** project in the **Solution Explorer**.
    * Right-click on **INSTALL** and select **Build**.
    * Repeat the above steps for the **Debug** configuration.

## OpenSSL:
* Download **Win64 OpenSSL** MSI file from [Shining Line Productions](https://slproweb.com/products/Win32OpenSSL.html).
* Extract the folder and place it in the `/lib`.

## wxWidgets
* Download wxWidgets ZIP from [wxWidgets Downloads](https://www.wxwidgets.org/downloads/).


* Unzip the folder and place it in a location of your choice.
* Set up the wxWidgets path in Windows environment variables:
    * Open **Environment Variables**.
    * In **System variables**, click **New**:
        * **Varialbe name**: WXWIN
        * **Variable value**: ```<your_path>\WxWidget```
    * Confirm by clicking OK to save changes.

# Server
## OpenCV
* Download OpenCV from [OpenCV Releases](https://opencv.org/releases/).
* Unzip the folder and place it in a suitable location.
* Set up the OpenCV path in Windows environment variables:
    * Open **Environment Variables**.
    * Under **Path**, click **New** and add:
        * ```<your_path>\opencv\build\x64\vc16\bin```
        * ```<your_path>\opencv\build\x64\vc16\lib```
    * Confirm by clicking OK to save changes.

### Run code (In Visual Studio 2022):
* Open the ```client``` or ```server``` folder in Visual Studio.
* Open ```CMakeLists.txt```and press **Ctrl+S** to configure the project.
* Build the project:
    * **F7**: Build all.
    * **Ctrl+F5**: Run with Release configuration.
    * **Shift+F5**: Run with Debug configuration.


## License

[MIT](https://choosealicense.com/licenses/mit/)
