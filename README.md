# stagit
> static git page generator

#### Information
This is my personal fork of [stagit](https://codemadness.org/stagit.html) which is running [git.acid.vegas](https://git.acid.vegas/)

## Dependencies
- [libgit2](https://github.com/libgit2/libgit2) *(v0.22+)*
- [md4c](https://github.com/mity/md4c) *(v0.4.4+)* 

## Features & Issues
###### Features
- Markdown rendering to HTML for README files
- ~~Syntax hilighting~~
- Repository categories
- Direct download to repository tar.gz
- Style changes
- ~~Raw file viewing~~

###### Issues
- [ ] Clickable heading *(h1-h6)* links in README *(md4c does not FULLY transform markdown)*
- [ ] Top/bottom padding for multi-lined `<code>` blocks 
- [ ] Missing image references in README files *(md4c does not look in /files/ for the .screens directory)*
- [ ] Generating html for binary content is pointless. No links at all for these files.

## Props
- Hiltjo Posthuma *(orignal author of [stagit](https://codemadness.org/git/stagit/))*
- Larry Burns *([stagit-md](https://github.com/lmburns/stagit-md))*
- Oscar Benedito *([md4c implementation](https://oscarbenedito.com/blog/2020/08/adding-about-pages-to-stagit/))*

___

###### Mirrors
[acid.vegas](https://git.acid.vegas/stagit) • [GitHub](https://github.com/acidvegas/stagit) • [GitLab](https://gitlab.com/acidvegas/stagit) • [SuperNETs](https://git.supernets.org/acidvegas/stagit)
