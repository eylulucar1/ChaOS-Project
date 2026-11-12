# 🖥️ OS_Project — 32-Bit Bare-Metal C++ & Assembly GUI Operating System

![Architecture](https://img.shields.io/badge/Architecture-x86%20(32--Bit)-blue?style=for-the-badge)
![Language](https://img.shields.io/badge/Language-C%2B%2B%20%7C%20Assembly-00599C?style=for-the-badge)
![Graphics](https://img.shields.io/badge/Graphics-VESA%201024x768x32-success?style=for-the-badge)
![Bootloader](https://img.shields.io/badge/Bootloader-GRUB%20Multiboot-orange?style=for-the-badge)
![License](https://img.shields.io/badge/Status-Chapter%201%20Completed-brightgreen?style=for-the-badge)

> **Dil Seçimi / Language Selection:**  
> [🇹🇷 **Türkçe Dokümantasyon**](#-türkçe-dokümantasyon) | [🇬🇧 **English Documentation**](#-english-documentation)

---

# 🇹🇷 Türkçe Dokümantasyon

## 👋 Merhaba! Bu Proje Neden Var?

Açık konuşalım: Sıfırdan bir işletim sistemi yazma fikri kulağa hem çok havalı hem de biraz korkutucu geliyor. İşin içine girdiğinizde fark ediyorsunuz ki; ekrana tek bir piksel çizdirmek, klavyeden basılan bir harfi okumak ya da fareyi hareket ettirmek için bile işlemciyle oldukça sıkı bir pazarlık yapmanız gerekiyor. Ortada ne hazır bir `printf`, ne `std::string`, ne de işletim sisteminin size sunduğu rahat koltuklar var; çünkü **işletim sisteminin kendisi sizsiniz.**

Bu projeye başlarken temel hedefimiz şuydu: Her işletim sisteminin olmazsa olmazı olan o ağır, zahmetli ama hayati "çekirdek tabanını" (Bootloader köprüsü, GDT, IDT, PIC, Dinamik RAM yönetimi, VESA ekran sürücüsü ve PS/2 klavye-fare protokolleri) temiz, modüler ve sağlam bir C++ mimarisiyle ayağa kaldırmak. Böylece sistemin "Triple Fault" verip sürekli kapanıp açıldığı karanlık ekranları geride bırakıp, işin en keyifli kısmına —yani kendi görsel tasarımımızı, pencerelerimizi ve uygulamalarımızı geliştirmeye— sağlam bir zemin hazırlamak.

Şu an incelediğiniz sürüm, bu yolculuğun **1. Bölümünü (Temel Çekirdek ve Grafiksel Masaüstü Tabanı)** temsil ediyor.

---

## Bölüm 1 — Neler Yapıldı? Ne İşe Yarıyor?

Projeyi hiç bilmeyen birinin de, doğrudan kaynak kodlara dalmak isteyen teknik bir geliştiricinin de rahatça anlayabilmesi için her modülü **"Kolay Anlatım"** ve **"Teknik Detay"** olarak iki boyutta ele aldık.

---

### 1. Önyükleme ve Çekirdeğe İlk Adım (`loader.s`, `linker.ld`, `Makefile`)

* **Basitçe Ne Yapıyor? :**  
  Bilgisayarın güç tuşuna bastığınızda işlemci henüz C++ dilinin ne olduğunu bilmez. GRUB adı verilen önyükleyici (bootloader), bizim işletim sistemimizi CD/ISO imajından bulur. `loader.s` dosyamız GRUB'a *"Merhaba, ben gerçek bir işletim sistemiyim ve lütfen ekranı 1024x768 yüksek çözünürlüklü renkli modda aç"* der. Ardından C++ kodlarımızın çalışabilmesi için bellekte güvenli bir çalışma alanı (Stack) hazırlar ve sözü ana C++ çekirdeğimize (`kernelMain`) bırakır.

* **Teknik Anlatım:**  
  * **Multiboot 1 Standardı:** `loader.s` içerisinde `0x1BADB002` sihirli sayısı (magic number), sayfa hizalama, bellek haritası ve VESA video modu bayrakları (`FLAGS = 0x07`) ile sağlama toplamı (`CHECKSUM`) `.multiboot` bölümüne yerleştirilmiştir.
  * **Bellek Yerleşimi (`linker.ld`):** Çekirdek fiziksel RAM'de `0x0100000` (1 MB) adresine konumlandırılır. `.init_array` bölümündeki global C++ nesnelerinin kurucu fonksiyonları (`start_ctors` - `end_ctors`) `callConstructors` rutini ile tetiklenir. Ayrıca çekirdeğin bittiği adres `kernel_end` sembolüyle işaretlenerek dinamik bellek (Heap) yöneticisine başlangıç noktası olarak sunulur.
  * **İzole Derleme (`Makefile`):** Standart kütüphanelerden tamamen bağımsız (`-nostdlib`, `-ffreestanding`, `-fno-exceptions`, `-fno-rtti`, `-fno-pie`) ve erken aşamada işlemci istisnasına yol açmaması için SSE/MMX komut setleri kapatılmış (`-mno-sse`, `-mno-sse2`) 32-bit ELF derleme hattı kurulmuştur.

---

### 2. Standart Kütüphane Olmadan Hayatta Kalmak (`types.h`, `string.h`, `string.cpp`)

* **Basitçe Ne Yapıyor? :**  
  Normalde C++ yazarken bir metni kopyalamak veya ekrana sayı yazdırmak için hazır kütüphaneler (`#include <iostream>`, `<cstring>`) kullanırız. Ancak kendi işletim sistemimizi yazdığımız için bu kütüphanelerin hiçbiri mevcut değil. Bu yüzden kendi metin ölçme, bellek kopyalama ve sayıları yazıya çevirme alet çantamızı sıfırdan kendimiz yazdık.

* **Teknik Anlatım :**  
  * Platformdan bağımsız sabit genişlikli tam sayı tipleri (`uint8_t`, `uint16_t`, `uint32_t`, `uint64_t`) ve GRUB'un VESA Framebuffer bilgilerini taşıyan `multiboot_info_t` (`__attribute__((packed))`) yapısı tanımlanmıştır.
  * Yüksek çözünürlüklü ekran tamponunu (Backbuffer) saniyede onlarca kez temizleyip kopyalayabilmek için 32-bit bloklar halinde çalışan optimize `memcpy` ve `memset32` fonksiyonları geliştirilmiştir.
  * Ayrıca `strlen`, `strcmp`, `strncmp`, `strcpy`, `strcat` ve tam sayıları ASCII dizelerine dönüştüren `int_to_str` ile `hex_to_str` fonksiyonları çekirdek bünyesine eklenmiştir.

---

### 3. İşlemciye Kuralları Öğretmek: GDT ve Port İletişimi (`gdt.h/.cpp`, `port.h/.cpp`)

* **Basitçe Ne Yapıyor?:**  
  * **GDT (Global Descriptor Table):** İşlemciye RAM'in hangi bölgelerinin çalıştırılabilir kod, hangi bölgelerinin ise veri olduğunu anlatan bir tapu sicil defteri gibidir. Ekran kartı belleğine erişirken işlemcinin *"Hop, oraya dokunamazsın!"* diyerek çökmesini engeller.
  * **Portlar (`port.cpp`):** İşlemcinin anakart üzerindeki diğer donanımlarla (klavye, fare, zamanlayıcı çipi) konuşmak için kullandığı telsiz kanallarıdır.

* **Teknik Anlatım :**  
  * **4 GB Doğrusal (Flat) Bellek Modeli:** 3.5 GB - 4 GB fiziksel adres aralığında bulunan VESA LFB (Linear Framebuffer) adreslerine erişirken *General Protection Fault* alınmaması için `codeSegmentSelector` (`0x10`) ve `dataSegmentSelector` (`0x18`) `0xFFFFFFFF` (4 GB) limitiyle 4 KB granülaritede (`0xC0`) yapılandırılmış ve `lgdt` + `ljmp` ile yüklenmiştir.
  * **I/O Port Soyutlaması:** `inb`/`outb`, `inw`/`outw` ve `inl`/`outl` satır içi Assembly komutları nesne yönelimli `Port8Bit`, `Port8BitSlow` (PIC gecikmeleri için), `Port16Bit` ve `Port32Bit` sınıflarına sarmalanmıştır.

---

### 4. Kesmeler (Interrupts), Zamanlayıcı (PIT) ve PCI Tarayıcı (`interrupts`, `timer`, `pci`)

* **Basitçe Ne Yapıyor? :**  
  İşlemcinin saniyede milyonlarca kez klavyeye dönüp *"Bir tuşa basıldı mı?"* diye sorması büyük bir vakit kaybıdır. Bunun yerine **Kesmeler (Interrupts)** kullanılır: Siz bir tuşa bastığınızda veya fareyi oynattığınızda donanım işlemcinin omzuna dokunur, işlemci o anki işini saniyenin binde biri kadar durdurup tuşu okur ve işine devam eder. **Timer (Zamanlayıcı)** ise sistemin kalp atışıdır; saniyede 100 kez atarak sistem saatini çalıştırır.

* **Teknik Anlatım :**  
  * **IDT ve 8259 PIC Yeniden Eşleme:** 256 girişlik *Interrupt Descriptor Table* kurulmuştur. Master (`0x20`/`0x21`) ve Slave (`0xA0`/`0xA1`) PIC denetleyicileri, işlemcinin ilk 32 iç istisnasıyla (CPU Exceptions `0x00`-`0x1F`) çakışmaması için `0x20` ofsetine yeniden eşlenmiştir.
  * **Assembly Kesme Köprüleri (`interruptstubs.s`):** Kesme anında tüm genel amaçlı yazmaçları (`eax`..`ebp`) yığına kaydedip C++ `InterruptManager::HandleInterrupt` metoduna aktaran ve işlem bitiminde `iret` ile bağlamı geri yükleyen makrolar yazılmıştır.
  * **PIT (Programmable Interval Timer):** `1193182 Hz` taban frekansı `0x40`/`0x43` portları üzerinden bölünerek `100 Hz` (10 ms) periyodik IRQ0 kesmesi üretilmiştir.
  * **PCI Denetleyicisi (`pci.cpp`):** `0xCF8` (komut) ve `0xCFC` (veri) portları üzerinden anakarttaki 8 veri yolu (bus), 32 aygıt (device) ve 8 fonksiyon taranarak donanım kimliklerini (`vendor_id`, `device_id`) ve BAR adreslerini tespit eden altyapı eklenmiştir.

---

### 5. Dinamik Bellek (RAM / Heap) Yöneticisi (`heap.h`, `heap.cpp`)

* **Basitçe Ne Yapıyor? :** 
  Masaüstünde yeni bir pencere açtığımızda veya büyük bir ekran tamponu oluşturduğumuzda o an RAM'den boş bir yer istememiz, pencereyle işimiz bittiğinde ise o yeri geri vermemiz gerekir. C++'taki `new` ve `delete` komutlarının çalışmasını sağlayan bu modül, işletim sistemimizin emlakçısıdır.

* **Teknik Anlatım :**   
  * Çekirdek bitiş adresinden (`kernel_end` / `0x400000`) itibaren 16 MB büyüklüğünde bir Heap alanı ayrılmıştır.
  * Çift yönlü bağlı liste (`MemoryChunk`) mimarisiyle çalışan `malloc` ve `free` algoritmaları yazılmıştır. Bellek serbest bırakıldığında (`free`), komşu boş bloklar otomatik olarak birleştirilerek (coalescing) bellek parçalanması (fragmentation) önlenir.
  * Global `operator new`, `operator new[]`, `operator delete` ve `operator delete[]` operatörleri doğrudan `MemoryManager::activeMemoryManager` üzerinden çalışacak şekilde aşırı yüklenmiştir.

---

### 6. Ekran Kartı (VESA), Font Motoru ve Klavye/Fare Sürücüleri (`vesa`, `font`, `keyboard`, `mouse`)

* **Basitçe Ne Yapıyor? :**
  * **VESA ve Çift Tamponlama (Double Buffering):** Pencereleri doğrudan ekrana çizersek pencereleri sürüklerken görüntüde rahatsız edici bir titreme (yırtılma) olur. Bunun yerine tüm masaüstünü önce arka planda görünmez bir tuvale (`backbuffer`) çiziyor, çizim bitince tek seferde gerçek ekrana aktarıyoruz. Sonuç: Yağ gibi akan pürüzsüz bir görüntü!
  * **Klavye ve Fare:** Fareden gelen ham sinyalleri ekrandaki `(X, Y)` koordinatlarına, klavyeden gelen elektrik sinyallerini (Scancode) ise büyük/küçük harf duyarlı gerçek harflere dönüştürür.

* **Teknik Anlatım :**  
  * **GraphicsContext (`vesa.cpp`):** 1024x768 çözünürlük ve 32-bit TrueColor (`0xRRGGBB`) renk derinliğinde çalışır. Sınır kontrollü piksel basma (`PutPixel`), optimize dikdörtgen doldurma (`FillRect`), Bresenham çizgi algoritması (`DrawLine`), 8x16 bitmap font tablosu (`font.h`) üzerinden metin işleme (`DrawString`) ve `SwapBuffers()` yeteneklerine sahiptir.
  * **PS/2 Klavye Sürücüsü (`keyboard.cpp`):** IRQ1 kesmesiyle `0x60` portundan gelen Scancode Set 1 verilerini okur. `Shift` ve `Caps Lock` durum makinelerini (`shiftPressed ^ capsLock`) takip ederek ASCII dönüşümü yapar ve odaklanılmış pencereye iletir.
  * **PS/2 Fare Sürücüsü (`mouse.cpp`):** İkinci PS/2 portunu (`0xA8`, `0xD4`) etkinleştirir ve IRQ12 üzerinden gelen 3 byte'lık paketleri senkronize eder. Donanım tamponu çakışmalarını önleyen zaman aşımı korumalı (`WaitSignal`, `WaitData`) el sıkışma protokolüne sahiptir.

---

### 7. Pencere Yöneticisi (GUI) ve 4 Yerleşik Uygulama (`gui/`, `apps/`)

* **Basitçe Ne Yapıyor? :**  
  İşletim sistemimizin vitrini burasıdır. Fareyle tutup sürükleyebildiğiniz gölgeli pencereler, tıkladığınız pencerenin en öne gelmesi, sağ üstten pencereyi kapatma (`X`) veya küçültme (`-`), sol alttaki `START` menüsü, sağ alttaki canlı dijital saat ve kutudan çıktığı haliyle çalışan 4 farklı uygulama!

* **Teknik Anlatım :**  
  * **GUI Omurgası (`widget.cpp`, `window.cpp`, `desktop.cpp`, `taskbar.cpp`):** Nesne yönelimli `Widget` -> `CompositeWidget` -> `Window` kalıtım hiyerarşisi kurulmuştur. `Desktop` sınıfı Z-sıralamasını (aktif pencereyi dizinin en önüne taşıma), piksel haritalı özel fare imlecini ve olay yönlendirmesini (Event Routing) yönetir.
  * **💻 Terminal (`terminal.cpp`):** Komut tamponu ve satır kaydırma destekli interaktif kabuk. `help`, `clear`, `about`, `mem` (anlık RAM kullanımı), `color` (tema rengi değiştirme) ve `echo <text>` komutlarını çalıştırır.
  * **📝 Notepad (`notepad.cpp`):** Çok satırlı metin yazma, `Enter` ile alt satıra geçme, `Backspace` ile silme ve üst bilgi çubuğunda canlı karakter sayacı (`Character Count`) özelliklerine sahip metin editörü.
  * **🎨 Paint (`paint.cpp`):** 7 renkli seçilebilir palet, `Clear` (tuvali temizleme) butonu ve fare sol tuşuyla serbest çizim yapılabilen piksel tabanlı çizim uygulaması.
  * **📊 System Monitor (`sysmon.cpp`):** `MemoryManager`, `ProgrammableIntervalTimer` ve `Desktop` sınıflarından aldığı canlı verilerle kullanılan/toplam RAM miktarını, yüzdelik doluluk çubuğunu (Progress Bar), sistemin çalışma süresini (Uptime), açık pencere sayısını ve anlık fare `(X, Y)` koordinatlarını gösteren sistem izleme aracı.

---

## 📂 Proje Klasör Mimarisi

Derleme çıktıları (`.o`, `.bin`, `.iso`) ile kaynak kodların birbirine karışmaması için proje üç ana katmana (`include/`, `src/`, `build/`) ayrılmıştır:

```text
OS_Project/
├── Makefile                        # Derleme, ISO oluşturma ve QEMU çalıştırma kuralları
├── linker.ld                       # 1 MB bellek yerleşimi ve bölüm hizalama betiği
├── include/                        # Başlık (.h) Dosyaları
│   ├── common/                     # types.h, string.h
│   ├── hardware/                   # gdt.h, port.h, interrupts.h, pci.h, timer.h
│   ├── memory/                     # heap.h
│   ├── drivers/                    # driver.h, vesa.h, font.h, keyboard.h, mouse.h
│   ├── gui/                        # theme.h, widget.h, window.h, taskbar.h, desktop.h
│   └── apps/                       # terminal.h, notepad.h, paint.h, sysmon.h
├── src/                            # Kaynak (.cpp ve .s) Dosyaları
│   ├── boot/                       # loader.s (Multiboot başlığı ve Stack kurulumu)
│   ├── common/                     # string.cpp
│   ├── hardware/                   # gdt.cpp, port.cpp, interruptstubs.s, interrupts.cpp, pci.cpp, timer.cpp
│   ├── memory/                     # heap.cpp
│   ├── drivers/                    # driver.cpp, vesa.cpp, keyboard.cpp, mouse.cpp
│   ├── gui/                        # widget.cpp, window.cpp, taskbar.cpp, desktop.cpp
│   ├── apps/                       # terminal.cpp, notepad.cpp, paint.cpp, sysmon.cpp
│   └── kernel.cpp                  # Ana çekirdek girişi (kernelMain)
└── build/                          # Derleme sırasında otomatik üretilen çıktı klasörü
```

---

## Kurulum ve Çalıştırma (Debian / Ubuntu)

Projeyi kendi bilgisayarınızda (veya sanal makinenizde) derleyip saniyeler içinde test etmek için aşağıdaki adımları izleyebilirsiniz:

### 1. Gerekli Araçları Kurun
Debian veya Ubuntu terminalinde derleyici, ISO oluşturucu ve QEMU emülatör paketlerini yükleyin:

```bash
sudo apt update && sudo apt install -y build-essential gcc-multilib g++-multilib xorriso grub-pc-bin grub-common qemu-system-x86
```

### 2. Derleyin ve QEMU ile Başlatın
Proje dizinine girip tek bir komut çalıştırmanız yeterlidir:

```bash
cd OS_Project
make clean && make run
```

* Sadece önyüklenebilir ISO dosyasını (`build/myos.iso`) üretmek için `make` komutunu kullanabilir, oluşan ISO'yu VirtualBox veya gerçek bir donanımda test edebilirsiniz.

---

## Sırada Ne Var? (Bölüm 2 Yol Haritası)

Çekirdek ve grafik tabanımız artık saat gibi çalıştığına göre, **2. Bölümde** odaklanacağımız hedefler şunlar:
* **Modern Görsel Tema (macOS / Windows Stili):** Yuvarlatılmış pencere köşeleri, renk geçişli (gradient) masaüstü duvar kağıdı, dairesel pencere kontrol butonları ve yeni ikon tasarımları.
* **Gelişmiş Sistem Uygulamaları:** Terminal için yeni komutlar (`calc`, `reboot`, `shutdown`) ve masaüstü kısayol ikonları.

---
---

# 🇬🇧 English Documentation

## Welcome! Why Does This Project Exist?

Let's be honest: building an operating system from scratch sounds both thrilling and a little intimidating. Once you dive into bare-metal programming, you quickly realize that even drawing a single pixel on the screen, reading a keystroke, or moving a mouse cursor requires negotiating directly with the CPU and motherboard hardware. There is no `<iostream>`, no `printf`, and no safety net provided by a host OS—because **you are the operating system.**

When we started **OS_Project**, our philosophy was simple: every graphical operating system relies on the same foundational "plumbing" (Bootloader handoff, GDT, IDT, PIC remapping, Dynamic Heap Memory, VESA Framebuffer driver, and PS/2 Keyboard/Mouse protocols). Instead of getting stuck in endless *Triple Fault* reboot loops, we engineered a clean, modular, object-oriented C++ and Assembly foundation from the ground up. With this rock-solid base in place, we can focus on the most creative part of OS development: crafting custom UI themes, window animations, and desktop applications.

What you see here is **Chapter 1: The Bare-Metal Kernel & Graphical Desktop Foundation**.

---

## Chapter 1 — What Has Been Built & How Does It Work?

To make this repository accessible to everyone—from curious beginners to seasoned systems programmers—we have broken down every subsystem into an **"In Plain English"** overview and an **"Under the Hood"** technical deep dive.

---

### 1. Bootstrapping & Entering the Kernel (`loader.s`, `linker.ld`, `Makefile`)

* **In Plain English (What does it do?):**  
  When you power on a computer, the processor doesn't immediately know how to run C++ code. First, the GRUB bootloader locates our OS inside the ISO image. Our `loader.s` file greets GRUB, proves it is a valid kernel, and asks GRUB to switch the monitor into a crisp `1024x768` 32-bit color graphics mode. It then sets up a 2 MB workspace in RAM (the Stack), initializes global C++ objects, and hands control over to `kernelMain`.

* **Under the Hood (Technical Details):**  
  * **Multiboot 1 Compliance:** `loader.s` defines a `.multiboot` section marked allocatable (`"a"`) containing the magic number `0x1BADB002`, flags (`0x07` for page alignment, memory info, and video mode), and a negated two's complement checksum.
  * **Linker Script (`linker.ld`):** Places the kernel at physical address `0x0100000` (1 MB). It preserves the `.init_array` section between `start_ctors` and `end_ctors` so `callConstructors()` can invoke global C++ constructors before `kernelMain`. It also exports `kernel_end` aligned to 4 KB to mark where free RAM begins.
  * **Freestanding Build Pipeline (`Makefile`):** Compiles 32-bit ELF objects (`-m32`) without standard libraries (`-nostdlib`, `-fno-builtin`, `-fno-exceptions`, `-fno-rtti`, `-fno-pie`) and explicitly disables SSE/MMX generation (`-mno-sse`, `-mno-sse2`) to prevent `Invalid Opcode` exceptions prior to FPU initialization.

---

### 2. Surviving Without the Standard Library (`types.h`, `string.h`, `string.cpp`)

* **In Plain English (What does it do?):**  
  In everyday C++ programming, you include standard headers to copy memory, measure text length, or convert numbers into text. In kernel space, none of those exist. This module is our custom Swiss Army knife that handles memory copying, fast screen clearing, and string manipulation.

* **Under the Hood (Technical Details):**  
  * Defines exact-width integer types (`uint8_t` through `uint64_t`) and the packed `multiboot_info_t` structure passed by GRUB in the `%ebx` register.
  * Implements 32-bit word-aligned `memcpy` and `memset32` routines capable of pushing megabytes of pixel data per frame for smooth double-buffered rendering.
  * Provides standalone implementations of `strlen`, `strcmp`, `strncmp`, `strcpy`, `strcat`, `int_to_str`, and `hex_to_str`.

---

### 3. Teaching the CPU the Rules: GDT & Hardware I/O Ports (`gdt.h/.cpp`, `port.h/.cpp`)

* **In Plain English (What does it do?):**  
  * **GDT (Global Descriptor Table):** Acts as the CPU's memory map and security guard. It defines which parts of RAM hold executable code and which parts hold data, ensuring the CPU doesn't crash when we write pixels to the graphics card's memory.
  * **I/O Ports (`port.cpp`):** Think of ports as walkie-talkie channels that the CPU uses to talk directly to hardware chips on the motherboard (like the keyboard controller, mouse, and system timer).

* **Under the Hood (Technical Details):**  
  * **4 GB Flat Protected Mode:** Because VESA Linear Framebuffers are typically mapped high in physical memory (around `0xE0000000` - `0xFD000000`), both the Kernel Code Segment (`0x10`) and Data Segment (`0x18`) are configured with a full `0xFFFFFFFF` (4 GB) limit and 4 KB page granularity (`0xC0`), loaded via `lgdt` and a far jump (`ljmp`).
  * **Object-Oriented Port I/O:** Wraps `inb`/`outb`, `inw`/`outw`, and `inl`/`outl` volatile inline assembly instructions into `Port8Bit`, `Port8BitSlow` (with I/O wait jumps for legacy PIC controllers), `Port16Bit`, and `Port32Bit` classes.

---

### 4. Interrupts (IDT & PIC), System Timer (PIT), and PCI Bus Scanner (`interrupts`, `timer`, `pci`)

* **In Plain English (What does it do?):**  
  Instead of the CPU wasting time constantly asking the keyboard *"Did the user press a key yet?"*, we use **Interrupts**. Whenever you press a key or move the mouse, the hardware taps the CPU on the shoulder. The CPU pauses for a microsecond, processes your input, and resumes its work. Meanwhile, the **Programmable Interval Timer (PIT)** acts as the system's heartbeat, ticking 100 times per second to power the taskbar clock and uptime counter.

* **Under the Hood (Technical Details):**  
  * **IDT & 8259 PIC Remapping:** Sets up a 256-entry *Interrupt Descriptor Table*. Remaps the Master (`0x20`) and Slave (`0xA0`) Programmable Interrupt Controllers to interrupt offset `0x20` (`32`–`47`) so hardware IRQs do not collide with Intel's reserved CPU exceptions (`0x00`–`0x1F`).
  * **Assembly ISR/IRQ Stubs (`interruptstubs.s`):** Saves processor state (`%ebp` through `%eax` and `%esp`) onto the stack, dispatches the interrupt number to `InterruptManager::HandleInterrupt`, sends End-of-Interrupt (`0x20`) signals to the PICs, and restores context via `iret`.
  * **PIT (`timer.cpp`):** Divides the `1,193,182 Hz` hardware oscillator over ports `0x40`/`0x43` to fire IRQ0 at exactly `100 Hz`.
  * **PCI Controller (`pci.cpp`):** Enumerates PCI configuration space via `0xCF8` and `0xCFC` to identify attached buses, devices, functions, and Base Address Registers (BARs).

---

### 5. Dynamic Memory Management — The Kernel Heap (`heap.h`, `heap.cpp`)

* **In Plain English (What does it do?):**  
  Whenever we open a new window or allocate a full-screen graphics buffer in RAM, the OS needs to find a free chunk of memory—and reclaim it when we're done. This module brings C++'s `new` and `delete` keywords to life on bare metal.

* **Under the Hood (Technical Details):**  
  * Initializes a 16 MB heap starting right after `kernel_end` (minimum `0x400000`).
  * Uses a doubly-linked list of `MemoryChunk` headers to manage allocations. When `free()` is called, adjacent unallocated chunks are automatically merged (coalesced) both forward and backward to prevent heap fragmentation.
  * Overloads global C++ `operator new`, `operator new[]`, `operator delete`, and `operator delete[]`.

---

### 6. VESA Graphics Engine, Bitmap Font & PS/2 Drivers (`vesa`, `font`, `keyboard`, `mouse`)

* **In Plain English (What does it do?):**  
  * **VESA & Double Buffering:** If we drew windows directly onto the monitor's memory, the screen would flicker terribly every time you dragged a window. Instead, we draw the entire desktop onto a hidden canvas in RAM (`backbuffer`) and copy the finished frame to the screen all at once (`SwapBuffers()`).
  * **Keyboard & Mouse Drivers:** Translates raw electrical packets from the PS/2 controller into smooth `(X, Y)` cursor movements, mouse clicks, and uppercase/lowercase ASCII text.

* **Under the Hood (Technical Details):**  
  * **GraphicsContext (`vesa.cpp`):** Drives the `1024x768x32` VESA Linear Framebuffer. Implements clipped pixel plotting, fast scanline rectangle fills (`FillRect`), Bresenham's line algorithm (`DrawLine`), and an 8x16 bitmap font renderer (`font.h`).
  * **PS/2 Keyboard (`keyboard.cpp`):** Handles IRQ1 (`0x21`), flushes stale controller output buffers during initialization, tracks `Shift` and `Caps Lock` states, and maps Scancode Set 1 bytes into ASCII characters.
  * **PS/2 Mouse (`mouse.cpp`):** Enables auxiliary PS/2 device streaming on IRQ12 (`0x2C`), synchronizes 3-byte movement/button packets (verifying the alignment bit `0x08`), and dispatches `OnMouseMove`, `OnMouseDown`, and `OnMouseUp` events.

---

### 7. Custom Window Manager (GUI) & 4 Built-in Applications (`gui/`, `apps/`)

* **In Plain English (What does it do?):**  
  This is the visual heart of **OS_Project**. It renders a complete desktop environment featuring a dotted grid background, a custom pixel-art cursor, draggable windows with drop shadows, active window z-ordering (clicking a window brings it to the front), close (`X`) and minimize (`-`) buttons, an interactive `START` menu, a live digital clock, and four built-in apps!

* **Under the Hood (Technical Details):**  
  * **Widget Hierarchy (`widget.cpp`, `window.cpp`, `desktop.cpp`, `taskbar.cpp`):** Built on an extensible `Widget` -> `CompositeWidget` -> `Window` architecture. `Desktop` manages focus, window z-order promotion, and taskbar integration.
  * **💻 Terminal (`terminal.cpp`):** Interactive command shell with scrolling line history. Supports `help`, `clear`, `about`, `mem` (queries live Heap usage), `color` (cycles terminal text palettes), and `echo <text>`.
  * **📝 Notepad (`notepad.cpp`):** Multi-line text editor supporting `Enter` line breaks, `Backspace` deletion, automatic line wrapping, and a live `Character Count` status bar.
  * **🎨 Paint (`paint.cpp`):** Interactive pixel canvas featuring a 7-color swatch selector, a `Clear` button, and continuous mouse-drag drawing.
  * **📊 System Monitor (`sysmon.cpp`):** Real-time diagnostics dashboard displaying dynamic RAM usage in KB and percentage, a color-coded visual progress bar, system uptime in seconds, active window count, and live mouse `(X, Y)` coordinates.

---

## Build & Run Instructions

You can compile and run **OS_Project** on any Debian/Ubuntu Linux environment (or VM):

### 1. Install Dependencies
```bash
sudo apt update && sudo apt install -y build-essential gcc-multilib g++-multilib xorriso grub-pc-bin grub-common qemu-system-x86
```

### 2. Compile & Launch in QEMU
```bash
cd OS_Project
make clean && make run
```

* To only build the bootable ISO image (`build/myos.iso`) without launching QEMU, simply run `make`.

---

## What's Next? (Chapter 2 Roadmap)

Now that the bare-metal kernel and windowing engine are complete, **Chapter 2** will focus on custom design and user experience:
* **Modern UI Overhaul (macOS / Windows Aesthetics):** Rounded window corners, gradient wallpapers, circular window control buttons, and desktop shortcut icons.
* **Expanded Applications:** Calculator (`calc`), power controls (`reboot`, `shutdown`), and new interactive UI widgets.
