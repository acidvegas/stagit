# stagit
> static git page generator

## Information
This is my personal fork of [stagit](https://codemadness.org/stagit.html) which is running [git.acid.vegas](https://git.acid.vegas/)

## Dependencies
- C compiler *(C99)*
- libc *(tested with OpenBSD, FreeBSD, NetBSD, Linux: glibc and musl)*
- [libgit2](https://github.com/libgit2/libgit2) *(v0.22+)*
- [md4c](https://github.com/mity/md4c) *(v0.4.4+)* 
- POSIX make *(optional)*

## Setup
```shell
cd stagit
make
sudo make install
```

###### New Features
- Markdown rendering to HTML for README files
- Syntax hilighting
- Repository categories
- Direct download to repository tar.gz
- Style changes

###### Props
- Hiltjo Posthuma *(orignal author of [stagit](https://codemadness.org/git/stagit/))*
- Larry Burns *([stagit-md](https://github.com/lmburns/stagit-md))*
- Oscar Benedito *([md4c implementation](https://oscarbenedito.com/blog/2020/08/adding-about-pages-to-stagit/))*