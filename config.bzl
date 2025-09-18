"""Configuration generation for libsl3"""

def _generate_config_impl(ctx):
    """Implementation for config header generation"""

    # Parse version string (format: 1.2.47002)
    version_parts = ctx.attr.version.split(".")
    if len(version_parts) != 3:
        fail("Version must be in format MAJOR.MINOR.PATCH (e.g., 1.2.47002)")

    major = version_parts[0]
    minor = version_parts[1]
    patch = int(version_parts[2])

    # Extract SQLite version from patch (47002 -> minor=47, patch=2)
    sqlite_minor = patch // 1000
    sqlite_patch = patch % 1000

    # Define substitutions
    substitutions = {
        "${sl3_MAJOR_VERSION}": major,
        "${sl3_MINOR_VERSION}": minor,
        "${sl3_PATCH_VERSION}": str(patch),
        "${sl3_VERSION}": ctx.attr.version,
        "${internal_SQLITE_MINOR_V}": str(sqlite_minor),
        "${internal_SQLITE_PATCH_V}": str(sqlite_patch),
    }

    # Use expand_template action instead of manual file reading
    ctx.actions.expand_template(
        template = ctx.file.template,
        output = ctx.outputs.config_header,
        substitutions = substitutions,
    )

generate_config = rule(
    implementation = _generate_config_impl,
    attrs = {
        "template": attr.label(allow_single_file = True, mandatory = True),
        "version": attr.string(mandatory = True),
        "config_header": attr.output(mandatory = True),
    },
)

def sl3_config(name, template, version, output):
    """Generate sl3 config header from template"""
    generate_config(
        name = name,
        template = template,
        version = version,
        config_header = output,
    )