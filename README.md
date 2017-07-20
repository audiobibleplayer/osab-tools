# OSAB - the Open Source Audio Bible player
## [audiobibleplayer.org](http://audiobibleplayer.org/)
#### Copyright (C) 2011-2017 Steven R. Patrick

OSAB is a design for a portable audio Bible player released under the MIT Licence in the hope that it will be used by others worldwide to produce and distribute portable audio players that are specifically designed to share God's word with those who need to hear.

The intention is to share firmware, software, schematics, PCB layout and enclosure design along with instructions on how to put it all together.  The intention is to design a PCB that can be hand-made and assembled by hand, but that could also be produced and assembled via normal mass production processes.  Likewise the intention with the enclosure is that it be 3D-printable on a low cost FDM 3D printer, or mass produced via classic injection moulding techniques.

## osab-tools
This repository is for tools for managing the audio content of OSAB.  This includes tools for conversion of audio formats (toogg), preparation of content index files (vs-chap) and uploading of audio content to a microSD card (makemenu and upload).

These tools are designed to work on a Linux machine.

### Licence
This is open source software released under the MIT Licence - please see the LICENCE file in this directory.

### Audio format
OSAB uses the VS1000 chip from vlsi.fi which plays Ogg Vorbis audio files (which typically have a .ogg suffix).

### Building and installing the Tools
Running:
```shell
make install
```
will build makemenu from source and then copy `makemenu`, `upload`, `vs-chap` and `toogg` to `/usr/local/bin`.  Check that `/usr/local/bin` is in your PATH.

### common dependencies
* GNU make

### makemenu dependencies
* gcc
* binutils

### upload dependencies
* perl with File::Copy module
* dosfstools

### vs-chap dependencies
* perl with File::Find::Rule module

### toogg dependencies
* ffmpeg
* vorbisgain
* md5sum

## Tool usage

### File naming convention
Before describing the use of each tool, it is good to understand the OSAB file naming convention.  It is not strictly required, but use of the vs-chap script for generating the content index file depends on it.  If you do not want to use vs-chap, then you can follow your own naming convention.  The convention is:
```shell
bb-bookname-ccc.ext
```
where:
  * bb is the book number padded with leading zero
  * bookname is the name of the book
  * ccc is the chapter number padded with leading zeros
  * ext is the file suffix, e.g. wav, mp3 or ogg

For example, the source files of first few chapters of the book of Genesis could be:
```shell
01-genesis-001.wav
01-genesis-002.wav
01-genesis-003.wav
```

The Ogg encoded file for the Third Epistle of John could be:
```shell
64-3john-001.ogg
```

### toogg
To assist in conversion from other file formats (e.g. wav, mp3, wma, etc) to ogg, a PERL script called `toogg` is provided.

#### usage
1. Make a processing directory for processing the audio files - e.g.:
```shell
mkdir english-kjv
```

2. Make a subdirectory of the processing directory that is named after the type of the source audio files (`wav` for wav files or `mp3` for mp3 files, etc) - e.g.:
```shell
mkdir english-kjv/mp3
```

3. Copy the source audio files into the latter subdirectory, e.g.:
```shell
cp -v /some_path/*.mp3 english-kjv/mp3
```

4. Run the toogg script in the processing directory and enter the required parameters.  The defaults are good.  I normally select mono for speech and stereo for music.  The following example is the processing of one mp3 chapter file - it would obviously be multiplied 1189 times for the entire Bible:
```shell
cd english-kjv
$ toogg
Working in directory english-kjv

Please enter the type of source file: flac, wav, mp3 or wma: mp3
File type is mp3

Directory mp3 already exists - make sure that it contains all the mp3 files that you want to convert.

Press enter to continue...

Enter the desired sample rate for the ogg files (e.g. 22050): 44100
Enter number of channels for the ogg files (1 for mono or 2 for stereo): 1
Enter quality factor for ogg files (-1 for lowest up to 10 for highest): 3

Converting the mp3 files to 44100 sample/sec 1 channel ogg files at quality factor 3...

ffmpeg version 3.1.8 Copyright (c) 2000-2017 the FFmpeg developers
  built with gcc 6.3.1 (GCC) 20161221 (Red Hat 6.3.1-1)
  configuration: --prefix=/usr --bindir=/usr/bin --datadir=/usr/share/ffmpeg --incdir=/usr/include/ffmpeg --libdir=/usr/lib64 --mandir=/usr/share/man --arch=x86_64 --optflags='-O2 -g -pipe -Wall -Werror=format-security -Wp,-D_FORTIFY_SOURCE=2 -fexceptions -fstack-protector-strong --param=ssp-buffer-size=4 -grecord-gcc-switches -specs=/usr/lib/rpm/redhat/redhat-hardened-cc1 -m64 -mtune=generic' --extra-ldflags='-Wl,-z,relro -specs=/usr/lib/rpm/redhat/redhat-hardened-ld' --enable-bzlib --disable-crystalhd --enable-fontconfig --enable-frei0r --enable-gcrypt --enable-gnutls --enable-ladspa --enable-libass --enable-libbluray --enable-libcdio --enable-indev=jack --enable-libfreetype --enable-libfribidi --enable-libgsm --enable-libmp3lame --enable-nvenc --extra-cflags=-I/usr/include/nvenc --enable-openal --enable-opencl --enable-opengl --enable-libopenjpeg --enable-libopus --enable-libpulse --enable-libschroedinger --enable-libsoxr --enable-libspeex --enable-libtheora --enable-libvorbis --enable-libv4l2 --enable-libvpx --enable-libx264 --enable-libx265 --enable-libxvid --enable-x11grab --enable-avfilter --enable-avresample --enable-postproc --enable-pthreads --disable-static --enable-shared --enable-gpl --disable-debug --disable-stripping --shlibdir=/usr/lib64 --enable-libmfx --enable-runtime-cpudetect
  libavutil      55. 28.100 / 55. 28.100
  libavcodec     57. 48.101 / 57. 48.101
  libavformat    57. 41.100 / 57. 41.100
  libavdevice    57.  0.101 / 57.  0.101
  libavfilter     6. 47.100 /  6. 47.100
  libavresample   3.  0.  0 /  3.  0.  0
  libswscale      4.  1.100 /  4.  1.100
  libswresample   2.  1.100 /  2.  1.100
  libpostproc    54.  0.100 / 54.  0.100
[mp3 @ 0x5580fe585320] Estimating duration from bitrate, this may be inaccurate
Input #0, mp3, from '01-genesis-001.mp3':
  Metadata:
    title           : Bible_01_genesis_001
    artist          : Holy Bible
    album           : Old Testament
  Duration: 00:06:17.24, start: 0.000000, bitrate: 16 kb/s
    Stream #0:0: Audio: mp3, 16000 Hz, mono, s16p, 16 kb/s
[ogg @ 0x5580fe595e60] Using AVStream.codec to pass codec parameters to muxers is deprecated, use AVStream.codecpar instead.
Output #0, ogg, to '../ogg/01-genesis-001.ogg':
  Metadata:
    title           : Bible_01_genesis_001
    artist          : Holy Bible
    album           : Old Testament
    encoder         : Lavf57.41.100
    Stream #0:0: Audio: vorbis (libvorbis), 44100 Hz, mono, fltp
    Metadata:
      encoder         : Lavc57.48.101 libvorbis
      title           : Bible_01_genesis_001
      artist          : Holy Bible
      album           : Old Testament
Stream mapping:
  Stream #0:0 -> #0:0 (mp3 (native) -> vorbis (libvorbis))
Press [q] to stop, [?] for help
size=    1911kB time=00:06:17.15 bitrate=  41.5kbits/s speed=66.6x    
video:0kB audio:1878kB subtitle:0kB other streams:0kB global headers:3kB muxing overhead: 1.801548%
Applying Replay Gain...

Analyzing files...

   Gain   |  Peak  | Scale | New Peak | Track
----------+--------+-------+----------+------
 -3.86 dB |  34448 |  0.64 |    22088 | 01-genesis-001.ogg

Calculating MD5 checksums...

a67c35c002d9ea474acaa548445cbe1c  01-genesis-001.ogg


DONE!
```

5. The toogg script will make a `ogg` directory in the processing directory that contains all the converted ogg files together with an `MD5SUM` file which lists the md5sums of all the ogg files.  The integrity of the files can be checked in the `ogg` directory with:
```shell
md5sum -c MD5SUM
```

### vs-chap
**makemenu** depends on a content index file with the following format:

```shell
TESTAMENT
BOOK
chapter-file
chapter-file
chapter-file

BOOK
chapter-file
chapter-file
```

For example:
```shell
TESTAMENT
BOOK
01-genesis-001.ogg
01-genesis-002.ogg
.
.
.
BOOK
/some_path//some_path/02-exodus-001.ogg
/some_path/02-exodus-002.ogg
.
.
.
TESTAMENT
BOOK
/some_path/19-psalms-001.ogg
.
.
.
BOOK
/some_path/19-psalms-030.ogg
.
.
.
BOOK
/some_path/19-psalms-060.ogg
.
.
.
TESTAMENT
BOOK
/some_path/40-matthew-001.ogg
.
.
.
BOOK
/some_path/66-revelation-001.ogg
.
.
.
/some_path/66-revelation-022.ogg
```

#### usage
`vs-chap` generates a content index file of the above format if the files are named according to the convention described above.  Use it as follows:
```shell
vs-chap <directory>
```
where the directory contains the ogg files or is the beginning of a path to a directory containing the ogg files.

The output is a file named `content.txt`.  It is best to rename it appropriately according to its content.

It can also be used on multiple directories like so:
```shell
vs-chap <directory1> <directory2> <directory3>...
```

The content index files can easily be concatenated:
```shell
cat Mandarin-CUV.txt English-KJV.txt Bible-study.txt Hymns.txt > Custom-content.txt
```

### upload
This is a PERL script that uses **makemenu** to read the content index file, generate a menu file called `menu.mnu`, copy `menu.mnu` along with all the content ogg files to a microSD card.

#### usage
1. Insert the microSD card into the Linux machine's card reader.

2. Check the device name assigned to the microSD card using `dmesg`:
```shell
dmesg | tail
dmesg | tail
[199557.562230] scsi host6: usb-storage 1-1.6:1.0
[199558.574707] scsi 6:0:0:0: Direct-Access     Generic- Multi-Card       1.00 PQ: 0 ANSI: 0 CCS
[199558.575805] sd 6:0:0:0: Attached scsi generic sg2 type 0
[199559.296208] sd 6:0:0:0: [sdb] 31116288 512-byte logical blocks: (15.9 GB/14.8 GiB)
[199559.297272] sd 6:0:0:0: [sdb] Write Protect is off
[199559.297275] sd 6:0:0:0: [sdb] Mode Sense: 03 00 00 00
[199559.298334] sd 6:0:0:0: [sdb] No Caching mode page found
[199559.298344] sd 6:0:0:0: [sdb] Assuming drive cache: write through
[199559.304187]  sdb:
[199559.307423] sd 6:0:0:0: [sdb] Attached SCSI removable disk
```
and/or using `lsblk`:
```shell
lsblk
NAME   MAJ:MIN RM   SIZE RO TYPE MOUNTPOINT
sdb      8:16   1  14.9G  0 disk /run/media/user/sdcard
sr0     11:0    1  1024M  0 rom  
sda      8:0    0 298.1G  0 disk 
├─sda4   8:4    0     1K  0 part 
├─sda2   8:2    0   476M  0 part /boot
├─sda5   8:5    0     8G  0 part [SWAP]
├─sda3   8:3    0   130G  0 part /home
├─sda1   8:1    0  48.8G  0 part /mnt/win
└─sda6   8:6    0 110.8G  0 part /
```
From both the above techniques it can be seen that the device name for the microSD card is `sdb`.

3. The basic form of using the script is:
```shell
upload <device_name> <content_file>
```

   For example:
```shell
upload sdb English-KJV.txt
/dev/sdb is mounted.
Unmounting /dev/sdb...
[sudo] password for srp:
Managed to unmount /dev/sdb on the first attempt
Kernel name = sdb
Chapter file = English-KJV.txt
Card size = 15931539456 bytes
Total size of all files: 3004552000 bytes
Available space on card: 15879733248 bytes
fatlabel: warning - lowercase labels might not work properly with DOS or Windows

Uploading files to microSD card...
sizing /narrations/vs1000/Bibles/tbi_kjv/ogg.44100.q3/01-genesis-001.ogg
sizing /narrations/vs1000/Bibles/tbi_kjv/ogg.44100.q3/01-genesis-002.ogg
sizing /narrations/vs1000/Bibles/tbi_kjv/ogg.44100.q3/01-genesis-003.ogg
.
.
.
sizing /narrations/vs1000/Bibles/tbi_kjv/ogg.44100.q3/66-revelation-22.ogg
18 50 40 27 36 34 24 21 4 31 24 22 25 29 36 10 13 10 42
25 29 30 30 30 31 31 12 8 66 52 5 48 12 14 3 9 1 4 7 3 3 3 2 14 4
27 28 16 24 21 28 16 16 13 6 6 4 4 5 3 6 4 3 1 13 5 5 3 5 1 1 1 22
Number of menu entries required = 1262
Number of menu entries required = 1262

entry	parent	subtree
Testaments
0	0	768
1	0	5376
2	0	11776

Books
3	0	18688
4	0	31488
6	0	48640
.
.
.
Chapters
73	768	0
74	768	0
75	768	0
.
.
.
Size of menu.mnu is 5048
Number of chapter files is 1189
Copying from /narrations/vs1000/Bibles/tbi_kjv/ogg.44100.q3/01-genesis-001.ogg to /mnt/sdb/00000001.ogg
Copying from /narrations/vs1000/Bibles/tbi_kjv/ogg.44100.q3/01-genesis-002.ogg to /mnt/sdb/00000002.ogg
Copying from /narrations/vs1000/Bibles/tbi_kjv/ogg.44100.q3/01-genesis-003.ogg to /mnt/sdb/00000003.ogg
.
.
.
Done copying files to SD card.
Managed to unmount /dev/sdb on the first attempt
Time difference is 241
Wrote 3004552000 bytes in 241 seconds
Average write speed: 11.9 MB/s
```

4. At this point the microSD card can be removed from the card reader and inserted into the card holder in the OSAB.
