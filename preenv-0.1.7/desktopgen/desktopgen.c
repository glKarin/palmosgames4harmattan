#include <stdlib.h>
#include <glib.h>
#include <glib-object.h>
#include <gio/gio.h>
#include <json-glib/json-glib.h>

#ifndef JSON_NODE_HOLDS
#define JSON_NODE_HOLDS(node,t) (json_node_get_node_type ((node)) == (t))
#endif

#ifndef JSON_NODE_HOLDS_OBJECT
#define JSON_NODE_HOLDS_OBJECT(node) (JSON_NODE_HOLDS ((node), JSON_NODE_OBJECT))
#endif

#ifndef JSON_NODE_HOLDS_VALUE
#define JSON_NODE_HOLDS_VALUE(node) (JSON_NODE_HOLDS ((node), JSON_NODE_VALUE))
#endif

#define DBUS_SERVICE_FILE_PATH "/usr/share/dbus-1/services/"
#define HILDON_DESKTOP_FILE_PATH "/usr/share/applications/hildon/"
#define HILDON_ICON_PATH "/usr/share/icons/hicolor/scalable/hildon/"
#define ITEM_PREFIX "preenv-"

static gchar ** input_files = NULL;
static gchar * preenv_path = NULL;
static gboolean dry_run = FALSE;
static gboolean do_remove = FALSE;

static GOptionEntry entries[] = {
	{ "preenv", 'p', 0, G_OPTION_ARG_FILENAME, &preenv_path, "Path to Preenv", "PATH" },
	{ "remove", 'r', 0, G_OPTION_ARG_NONE, &do_remove, "Remove the files instead of generating them", NULL },
	{ "dry-run", 'n', 0, G_OPTION_ARG_NONE, &dry_run, "Print what the program would do instead of doing it", NULL },
	{ G_OPTION_REMAINING, 0, 0, G_OPTION_ARG_FILENAME_ARRAY, &input_files, "Path to appinfo.json file", "FILE" },
	{ NULL }
};

static gchar * wrapper_path = NULL;

typedef struct AppInfo {
	JsonObject *info;
	gchar *input_path;
	gchar *dbus_id;
	gchar *binary_path;
	gchar *src_icon_path;
} AppInfo;

static const gchar * get_appinfo_key(AppInfo* app, const gchar *key)
{
	JsonNode *node = json_object_get_member(app->info, key);
	if (node && JSON_NODE_HOLDS_VALUE(node)) {
		return json_node_get_string(node);
	} else {
		g_printerr("Not a string attribute: %s\n", key);
		return NULL;
	}
}

static void write_file(const gchar *file, const gchar *contents)
{
	if (dry_run) {
		g_print("Would create \"%s\" with:\n%s\n", file, contents);
	} else {
		GError *error = NULL;
		if (g_file_set_contents(file, contents, -1, &error)) {
			g_print("Wrote \"%s\"\n", file);
		} else {
			g_printerr("Cannot write file \"%s\": %s\n", file, error->message);
			exit(4);
		}
	}
}

static void copy_file(const gchar *file, const gchar *dest)
{
	if (dry_run) {
		g_print("Would copy \"%s\" to \"%s\"\n", file, dest);
	} else {
		GFile *src = g_file_new_for_path(file);
		GFile *dst = g_file_new_for_path(dest);
		GError *error = NULL;
		if (g_file_copy(src, dst, G_FILE_COPY_OVERWRITE,
			NULL, NULL, NULL, &error)) {
			g_print("Wrote \"%s\"\n", dest);
		} else {
			g_printerr("Cannot copy file \"%s\" to \"%s\": %s\n",
				file, dest, error->message);
			exit(4);
		}
		g_object_unref(dst);
		g_object_unref(src);
	}
}

static void remove_file(const gchar *file)
{
	if (dry_run) {
		g_print("Would remove \"%s\"\n", file);
	} else {
		GFile *src = g_file_new_for_path(file);
		GError *error = NULL;
		if (g_file_delete(src, NULL, &error) != 0) {
			g_print("Deleted \"%s\"\n", file);
		} else {
			g_printerr("Cannot delete file \"%s\": %s\n",
				file, error->message);
			g_print("Continuing...\n");
		}
		g_object_unref(src);
	}
}

static void global_init_and_check()
{
	if (!preenv_path) {
		preenv_path = g_strdup(g_getenv("PREENV_ROOT"));
	}
	if (!preenv_path) {
		g_printerr("Cannot find PREENV_ROOT. You must set one with '-p'\n");
		exit(1);
	}
	if (!g_file_test(preenv_path, G_FILE_TEST_IS_DIR)) {
		g_printerr("Cannot access Preenv directory: %s\n", preenv_path);
		exit(3);
	}
	wrapper_path = g_build_filename(preenv_path, "wrapper.sh", NULL);
	if (!g_file_test(wrapper_path, G_FILE_TEST_IS_EXECUTABLE)) {
		g_printerr("Wrapper script is not executable: %s\n", wrapper_path);
		exit(3);
	}
}

static void sanity_check(AppInfo *app)
{
	const gchar *id = get_appinfo_key(app, "id");
	if (!id) {
		g_printerr("'%s' attribute is mandatory\n", "id");
		exit(2);
	}
	const gchar *bin = get_appinfo_key(app, "main");
	if (!bin) {
		g_printerr("'%s' attribute is mandatory\n", "main");
		exit(2);
	}
	const gchar *type = get_appinfo_key(app, "type");
	if (!type || (g_ascii_strcasecmp(type, "game") != 0 &&
		g_ascii_strcasecmp(type, "pdk")) != 0) {
		g_printerr("'%s' attribute is mandatory (and should be '%s')\n",
			"type", "pdk");
		exit(2);
	}
	gchar *binpath = g_build_filename(app->input_path, bin, NULL);
	if (!g_file_test(binpath, G_FILE_TEST_IS_EXECUTABLE)) {
		g_printerr("'%s' is not executable\n", binpath);
		exit(3);
	}
	if (!get_appinfo_key(app, "version")) {
		g_printerr("'%s' attribute is mandatory\n", "version");
		exit(2);
	}
	if (!get_appinfo_key(app, "vendor")) {
		g_printerr("'%s' attribute is mandatory\n", "vendor");
		exit(2);
	}
	if (!get_appinfo_key(app, "title")) {
		g_printerr("'%s' attribute is mandatory\n", "title");
		exit(2);
	}
	const gchar *icon = get_appinfo_key(app, "icon");
	if (icon) {
		if (!g_str_has_suffix(icon, ".png")) {
			g_printerr("Currently, icon must be a png file\n");
			exit(2);
		}
	}
	g_free(binpath);
}

static void fill_information(AppInfo *app)
{
	const gchar *id = get_appinfo_key(app, "id");
	const gchar *bin = get_appinfo_key(app, "main");
	const gchar *icon = get_appinfo_key(app, "icon");
	app->binary_path = g_build_filename(app->input_path, bin, NULL);
	/* Must be a valid D-BUS service name */
	app->dbus_id = g_strcanon(g_strdup(id),
		G_CSET_A_2_Z G_CSET_a_2_z G_CSET_DIGITS "_.", '_');
	if (icon) {
		app->src_icon_path = g_build_filename(app->input_path, icon, NULL);
	} else {
		app->src_icon_path = NULL;
	}
}

static void icon_sanity_check(AppInfo *app)
{
	if (app->src_icon_path) {
		if (!g_file_test(app->src_icon_path, G_FILE_TEST_EXISTS)) {
			g_printerr("'%s' does not exist\n", app->src_icon_path);
			exit(3);
		}
	}
}

static void free_information(AppInfo *app)
{
	g_free(app->binary_path);
	g_free(app->dbus_id);
	g_free(app->src_icon_path);
}

static void copy_icon(const gchar *srciconpath, const gchar *dsttitle)
{
	gchar *dsticonname = g_strdup_printf("%s.png", dsttitle);
	gchar *dsticonpath = g_build_filename(HILDON_ICON_PATH, dsticonname, NULL);

	copy_file(srciconpath, dsticonpath);

	g_free(dsticonpath);
	g_free(dsticonname);
}

static void remove_app(AppInfo *app)
{
	gchar *file = g_strdup_printf(
		HILDON_DESKTOP_FILE_PATH ITEM_PREFIX "%s.desktop", app->dbus_id);

	remove_file(file);
	g_free(file);

	file = g_strdup_printf(
		DBUS_SERVICE_FILE_PATH ITEM_PREFIX "%s.service", app->dbus_id);
	remove_file(file);
	g_free(file);

	if (app->src_icon_path) {
		file = g_strdup_printf(
			HILDON_ICON_PATH ITEM_PREFIX "%s.png", app->dbus_id);
		remove_file(file);
		g_free(file);
	}
}

static void build_service_file(AppInfo *app)
{
	gchar *file = g_strdup_printf(
		DBUS_SERVICE_FILE_PATH ITEM_PREFIX "%s.service", app->dbus_id);
	gchar *contents = g_strdup_printf(
"[D-BUS Service]\n"
"Name=%s\n"
"Exec=\"%s\" \"%s\" \"%s\"\n"
, app->dbus_id, wrapper_path, app->dbus_id, app->binary_path);
	write_file(file, contents);
	g_free(file);
	g_free(contents);
}

static void build_desktop_file(AppInfo* app)
{
	gchar *file = g_strdup_printf(
		HILDON_DESKTOP_FILE_PATH ITEM_PREFIX "%s.desktop", app->dbus_id);
	GString *contents = g_string_sized_new(510);
	g_string_printf(contents,
"[Desktop Entry]\n"
"Encoding=UTF-8\n"
"Version=1.0\n"
"Type=Application\n"
"Name=%s\n"
"Exec=\"%s\" \"%s\" \"%s\"\n"
"X-Osso-Type=application/x-executable\n"
"X-Osso-Service=%s\n"
"X-Preenv-Generated=true\n"
"X-Preenv-Vendor=%s\n"
"X-Preenv-Version=%s\n"
, get_appinfo_key(app, "title"),
	wrapper_path, app->dbus_id, app->binary_path, app->dbus_id,
	get_appinfo_key(app, "vendor"), get_appinfo_key(app, "version")
);
	if (app->src_icon_path) {
		gchar * icon_title = g_strdup_printf(ITEM_PREFIX "%s", app->dbus_id);
		copy_icon(app->src_icon_path, icon_title);
		g_string_append_printf(contents, "Icon=%s\n", icon_title);
		g_free(icon_title);
	}
	write_file(file, contents->str);
	g_free(file);
	g_string_free(contents, TRUE);
}

static void process_file(const gchar * input_file)
{
	GError *error = NULL;
	AppInfo app;
	JsonParser *parser;
	JsonNode *root;
	gchar * input_path = g_path_get_dirname(input_file);

	parser = json_parser_new();
	json_parser_load_from_file(parser, input_file, &error);
	if (error) {
		g_printerr("Unable to parse \"%s\": %s\n", input_file, error->message);
		exit(2);
	}

	root = json_parser_get_root(parser);
	if (!JSON_NODE_HOLDS_OBJECT(root)) {
		g_printerr("JSON file is not an object\n");
		exit(2);
	}

	/* Fill in basic struct AppInfo fields */
	app.info = json_node_get_object(root);
	app.input_path = realpath(input_path, NULL);

	sanity_check(&app);
	fill_information(&app);
	if (!do_remove) {
		icon_sanity_check(&app);
		build_service_file(&app);
		build_desktop_file(&app);
	} else {
		remove_app(&app);
	}
	free_information(&app);

	/* Free basic struct AppInfo fields */
	free(app.input_path);

	g_free(input_path);
	g_object_unref(parser);
}

int main(int argc, char ** argv)
{
	GError *error = NULL;
	GOptionContext *context;

	g_type_init();

	context = g_option_context_new("- generate Hildon desktop files from WebOS appinfo");
	g_option_context_add_main_entries(context, entries, NULL);

	if (!g_option_context_parse(context, &argc, &argv, &error)) {
		g_printerr("Invalid options passed: %s\n", error->message);
		return 1;
	}

	global_init_and_check();

	if (input_files) {
		for (gint i = 0; input_files[i]; i++) {
			process_file(input_files[i]);
		}
	} else {
		g_printerr("Warning: no files processed\n");
	}

	return 0;
}
