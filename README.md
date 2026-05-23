# tarsau Arsivleme Programi

Bu proje, Sistem Programlama dersi icin C dili ile yazilmis, sikistirma yapmadan metin dosyalarini `.sau` arsiv dosyasinda birlestiren ve tekrar acan komut satiri programidir.

## Derleme

```bash
make
```

## Arsiv Olusturma

```bash
./tarsau -b test_files/t1.txt test_files/t2.txt test_files/t3.dat -o s1.sau
```

`-o` verilmezse varsayilan arsiv adi `a.sau` olur.

## Arsiv Acma

```bash
./tarsau -a s1.sau d1
```

Dizin belirtilmezse dosyalar gecerli dizinde acilir.

## Temizlik

```bash
make clean
```
## Error Controls
- Invalid archive file check
- ASCII file validation
- Maximum file limit control

## Development Note
Archive creation and extraction steps were tested again.
