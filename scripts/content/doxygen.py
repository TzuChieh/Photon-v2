from pathlib import Path


def common_config_path() -> Path:
    script_dir = Path(__file__).parent.parent
    root_dir = script_dir.parent
    main_dir = root_dir / "Main"
    return main_dir / "Common_doxygen.config"

def create_base_config(dir, doxygen_cwd, project_name, project_code_name):
    """
    @param dir The directory to place the created config file.
    @param doxygen_cwd The directory where doxygen will be executed from.
    @param project_name Human readable name of the project.
    @param project_code_name Name used internally to identify the project.
    """
    doxygen_cwd = Path(doxygen_cwd).absolute()
    file_path = Path(dir) / "Base_doxygen.config"
    common_config_rel_path = common_config_path().relative_to(doxygen_cwd, walk_up=True)
    common_config_rel_dir = common_config_rel_path.parent
    output_rel_dir = common_config_rel_dir / "docs" / project_code_name
    header_rel_dir = (common_config_rel_dir / "header").with_suffix('.html')
    css1_rel_path = (common_config_rel_dir / "doxygen-awesome-css" / "doxygen-awesome").with_suffix('.css')
    css2_rel_path = (common_config_rel_dir / "custom").with_suffix('.css')
    toc_js_rel_path = (common_config_rel_dir / "doxygen-awesome-css" / "doxygen-awesome-interactive-toc").with_suffix('.js')
    layout_rel_path = (common_config_rel_dir / "doxygen_layout").with_suffix('.xml')
    bibtex1_rel_path = (common_config_rel_dir / "strings-full").with_suffix('.bib')
    bibtex2_rel_path = (common_config_rel_dir / "bibtex").with_suffix('.bib')
    tag_rel_path = (common_config_rel_dir / "docs" / project_code_name).with_suffix('.tag')

    with open(file_path, 'w') as f:
        f.write(f"@INCLUDE = {common_config_rel_path.as_posix()}\n")
        f.write("\n")
        f.write(f"PROJECT_NAME          = \"{project_name}\"\n")
        f.write(f"OUTPUT_DIRECTORY      = {output_rel_dir.as_posix()}\n")
        f.write(f"HTML_HEADER           = {header_rel_dir.as_posix()}\n")
        f.write(f"HTML_EXTRA_STYLESHEET = {css1_rel_path.as_posix()} \\\n")
        f.write(f"                        {css2_rel_path.as_posix()}\n")
        f.write(f"HTML_EXTRA_FILES      = {toc_js_rel_path.as_posix()}\n")
        f.write(f"LAYOUT_FILE           = {layout_rel_path.as_posix()}\n")
        f.write(f"CITE_BIB_FILES        = {bibtex1_rel_path.as_posix()} \\\n")
        f.write(f"                        {bibtex2_rel_path.as_posix()}\n")
        f.write(f"GENERATE_TAGFILE      = {tag_rel_path.as_posix()}\n")
