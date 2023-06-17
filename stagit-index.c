#include <err.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <git2.h>

static git_repository *repo;
static const char *relpath = "";
static char description[255] = "Acidvegas Repositories";
static char *name = "";
static char category[255];

/* Handle read or write errors for a FILE * stream */
void checkfileerror(FILE *fp, const char *name, int mode) {
	if (mode == 'r' && ferror(fp))
		errx(1, "read error: %s", name);
	else if (mode == 'w' && (fflush(fp) || ferror(fp)))
		errx(1, "write error: %s", name);
}

void joinpath(char *buf, size_t bufsiz, const char *path, const char *path2) {
	int r;
	r = snprintf(buf, bufsiz, "%s%s%s", path, path[0] && path[strlen(path) - 1] != '/' ? "/" : "", path2);
	if (r < 0 || (size_t)r >= bufsiz)
		errx(1, "path truncated: '%s%s%s'", path, path[0] && path[strlen(path) - 1] != '/' ? "/" : "", path2);
}

/* Percent-encode, see RFC3986 section 2.1. */
void percentencode(FILE *fp, const char *s, size_t len) {
	static char tab[] = "0123456789ABCDEF";
	unsigned char uc;
	size_t i;
	for (i = 0; *s && i < len; s++, i++) {
		uc = *s;
		/* NOTE: do not encode '/' for paths or ",-." */
		if (uc < ',' || uc >= 127 || (uc >= ':' && uc <= '@') || uc == '[' || uc == ']') {
			putc('%', fp);
			putc(tab[(uc >> 4) & 0x0f], fp);
			putc(tab[uc & 0x0f], fp);
		} else {
			putc(uc, fp);
		}
	}
}

/* Escape characters below as HTML 2.0 / XML 1.0. */
void xmlencode(FILE *fp, const char *s, size_t len) {
	size_t i;
	for (i = 0; *s && i < len; s++, i++) {
		switch(*s) {
			case '<':  fputs("&lt;",   fp); break;
			case '>':  fputs("&gt;",   fp); break;
			case '\'': fputs("&#39;" , fp); break;
			case '&':  fputs("&amp;",  fp); break;
			case '"':  fputs("&quot;", fp); break;
			default:   putc(*s, fp);
		}
	}
}

void printtimeshort(FILE *fp, const git_time *intime) {
	struct tm *intm;
	time_t t;
	char out[32];
	t = (time_t)intime->time;
	if (!(intm = gmtime(&t)))
		return;
	strftime(out, sizeof(out), "%Y-%m-%d", intm);
	fputs(out, fp);
}

void writeheader(FILE *fp) {
	fputs("<!DOCTYPE html>\n<meta charset=\"UTF-8\">\n<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">\n<title>", fp);
	xmlencode(fp, description, strlen(description));
	fputs("</title>\n<meta name=\"description\" content=\"acidvegas repositories\">\n"
		"<meta name=\"keywords\" content=\"git, repositories, supernets, irc, python, stagit\">\n"
		"<meta name=\"author\" content=\"acidvegas\">\n", fp);
	fputs("<link rel=\"icon\" type=\"image/png\" href=\"/assets/favicon.png\">\n"
		"<link rel=\"stylesheet\" type=\"text/css\" href=\"/assets/style.css\">\n", fp);
	fputs("<center>\n<img src=\"/assets/acidvegas.png\"><br>\n<img src=\"/assets/mostdangerous.png\"><br><br>\n", fp);
	fputs("<div class=\"container\">\n\t<center>\n\t<table>\n\t\t<tr><td>\n"
		"<b>contact</b> : <a href=\"https://discord.gg/BCqRZZR\">discord</a> &bull; <a href=\"ircs://irc.supernets.org/superbowl\">irc</a> &bull; <a href=\"mailto://acid.vegas@acid.vegas\">mail</a> &bull; <a href=\"https://twitter.com/acidvegas\">twitter</a>\n"
		"<br><b>mirrors</b> : <a href=\"https://github.com/acidvegas\">github</a> &bull; <a href=\"https://gitlab.com/acidvegas\">gitlab</a> &bull; <a href=\"https://git.sr.ht/~acidvegas\">sourcehut</a> &bull; <a href=\"https://git.supernets.org/acidvegas\">supernets</a>\n"
        "\t\t</td></tr>\n\t</table>\n\t</center>\n</div>\n<br>\n", fp);
	fputs("<div id=\"content\">\n\t<table id=\"index\">\n\t\t<thead>\n\t\t\t<tr><td>Name</td><td>Description</td><td>Last commit</td></tr>\n\t\t</thead>\n\t\t<tbody>", fp);
}

void writefooter(FILE *fp) {
	fputs("\n\t\t</tbody>\n\t</table>\n</div>\n<div id=\"footer\">\n"
		"\t&copy; 2023 acidvegas, inc &bull; generated with stagit\n"
		"</div>\n</center>", fp);
}

int writelog(FILE *fp) {
	git_commit *commit = NULL;
	const git_signature *author;
	git_revwalk *w = NULL;
	git_oid id;
	char *stripped_name = NULL, *p;
	int ret = 0;

	git_revwalk_new(&w, repo);
	git_revwalk_push_head(w);

	if (git_revwalk_next(&id, w) ||
	    git_commit_lookup(&commit, repo, &id)) {
		ret = -1;
		goto err;
	}

	author = git_commit_author(commit);

	/* strip .git suffix */
	if (!(stripped_name = strdup(name)))
		err(1, "strdup");
	if ((p = strrchr(stripped_name, '.')))
		if (!strcmp(p, ".git"))
			*p = '\0';

	fputs("\n\t\t\t<tr class=\"item-repo\"><td><a href=\"", fp);
	percentencode(fp, stripped_name, strlen(stripped_name));
	fputs("/log.html\">", fp);
	xmlencode(fp, stripped_name, strlen(stripped_name));
	fputs("</a></td><td>", fp);
	xmlencode(fp, description, strlen(description));
	fputs("</td><td>", fp);
	if (author)
		printtimeshort(fp, &(author->when));
	fputs("</td></tr>", fp);

	git_commit_free(commit);
err:
	git_revwalk_free(w);
	free(stripped_name);

	return ret;
}

int main(int argc, char *argv[]) {
	FILE *fp;
	char path[PATH_MAX], repodirabs[PATH_MAX + 1];
	const char *repodir;
	int i, ret = 0;

	if (argc < 2) {
		fprintf(stderr, "usage: %s [repodir...]\n", argv[0]);
		return 1;
	}

	/* do not search outside the git repository:
	   GIT_CONFIG_LEVEL_APP is the highest level currently */
	git_libgit2_init();
	for (i = 1; i <= GIT_CONFIG_LEVEL_APP; i++)
		git_libgit2_opts(GIT_OPT_SET_SEARCH_PATH, i, "");
	/* do not require the git repository to be owned by the current user */
	git_libgit2_opts(GIT_OPT_SET_OWNER_VALIDATION, 0);

#ifdef __OpenBSD__
	if (pledge("stdio rpath", NULL) == -1)
		err(1, "pledge");
#endif

	writeheader(stdout);

	for (i = 1; i < argc; i++) {
		if (!strcmp(argv[i], "-c")) {
			i++;
			if (i == argc)
				err(1, "missing argument");
			repodir = argv[i];
			fputs("\n\t\t\t<tr class=\"category\"><td colspan=\"3\">", stdout);
			xmlencode(stdout, repodir, strlen(repodir));
			fputs("</td></tr>", stdout);
			continue;
		}

		repodir = argv[i];
		if (!realpath(repodir, repodirabs))
			err(1, "realpath");

		if (git_repository_open_ext(&repo, repodir, GIT_REPOSITORY_OPEN_NO_SEARCH, NULL)) {
			fprintf(stderr, "%s: cannot open repository\n", argv[0]);
			ret = 1;
			continue;
		}

		/* use directory name as name */
		if ((name = strrchr(repodirabs, '/')))
			name++;
		else
			name = "";

		/* read description or .git/description */
		joinpath(path, sizeof(path), repodir, "description");
		if (!(fp = fopen(path, "r"))) {
			joinpath(path, sizeof(path), repodir, ".git/description");
			fp = fopen(path, "r");
		}
		description[0] = '\0';
		if (fp) {
			if (!fgets(description, sizeof(description), fp))
				description[0] = '\0';
			checkfileerror(fp, "description", 'r');
			fclose(fp);
		}
		writelog(stdout);
	}
	writefooter(stdout);

	/* cleanup */
	git_repository_free(repo);
	git_libgit2_shutdown();

	checkfileerror(stdout, "<stdout>", 'w');

	return ret;
}
