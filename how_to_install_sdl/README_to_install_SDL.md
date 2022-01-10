# برای نصب SDL
## linux:
###### `sudo apt install libsdl2-dev libsdl2-gfx-dev`

## mac:

اگر برو رو نصب نکردین:
[homebrew](https://brew.sh/ "brew.sh")

و بعد از اون:
###### `brew install Sdl2`
###### `brew install Sdl2Gfx`

## windows:

ویندوز جای واحدی نداره که کل کتابخونه ها اونجا جمع شده باشه فقط کافیه ریپوزیتوری رو کلون کنید و شروع کنید کد زدن. که البته توصیه میشه از لینوکس استفاده کنید.

# اضافه کردن فایل کد به پروژه
ازین به بعد هر فایل کدی خواستید اضافه کنید تو فولدر اس آر سی قرار بدید

# CMake(Build and Run)
کافیه بعد از کلون کردن کل ریپو یه فایل با اسم زیر تو فولدر project
###### `CMakeLists.txt`
بسازید و بعد
با توجه به سیستم عاملتون کل محتوای یکی از سه تا فایل
###### `cmake_win.txt, cmake_linux.txt, cmake_mac.txt`
رو تو اون فایل جدیدی که ساختین کپی کنید.
 حالا به جای تمام جاهایی که 
###### `project_name`
تو اون فایل اومده، یه اسم دلخواه برا پروژه تون رو جایگزین کنید

اگر از ویندوز استفاده میکنید لازمه که سه خط کد زیر رو بالای تابع مین برنامه تون قرار بدید.
###### `#ifdef main`
###### `#undef main`
###### `#endif`

---
بعد میتونید با استفاده از دستور
###### `cmake --build .`
یا از داخل سی لاین با استفاده از چکش سبز رنگی که سمت بالاراست پنجره است، پروژه رو بیلد کنید و با استفاده از مثلث سبز رنگ اونو اجرا کنید.