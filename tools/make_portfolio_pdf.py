from pathlib import Path
import textwrap


PAGE_WIDTH = 595
PAGE_HEIGHT = 842
MARGIN = 45
FONT_SIZE = 10
LINE_HEIGHT = 14
MAX_CHARS = 92


def pdf_escape(text: str) -> str:
    return text.replace("\\", "\\\\").replace("(", "\\(").replace(")", "\\)")


def markdown_to_lines(markdown: str) -> list[str]:
    lines: list[str] = []
    in_code = False

    for raw_line in markdown.splitlines():
        line = raw_line.rstrip()

        if line.startswith("```"):
            in_code = not in_code
            lines.append("")
            continue

        if not line:
            lines.append("")
            continue

        if not in_code:
            if line.startswith("# "):
                line = line[2:].upper()
                lines.extend(["", line])
                continue
            if line.startswith("## "):
                line = line[3:].upper()
                lines.extend(["", line])
                continue
            if line.startswith("### "):
                line = line[4:]
                lines.extend(["", line])
                continue
            if line.startswith("- "):
                line = "* " + line[2:]

        prefix = "    " if in_code else ""
        for wrapped in textwrap.wrap(
            line,
            width=MAX_CHARS - len(prefix),
            replace_whitespace=False,
            drop_whitespace=False,
        ):
            lines.append(prefix + wrapped)

    return lines


def paginate(lines: list[str]) -> list[list[str]]:
    lines_per_page = int((PAGE_HEIGHT - 2 * MARGIN) / LINE_HEIGHT)
    pages: list[list[str]] = []
    current: list[str] = []

    for line in lines:
        if len(current) >= lines_per_page:
            pages.append(current)
            current = []
        current.append(line)

    if current:
        pages.append(current)

    return pages


def content_stream(lines: list[str]) -> bytes:
    commands = [
        "BT",
        f"/F1 {FONT_SIZE} Tf",
        f"1 0 0 1 {MARGIN} {PAGE_HEIGHT - MARGIN} Tm",
    ]

    for index, line in enumerate(lines):
        if index:
            commands.append(f"0 -{LINE_HEIGHT} Td")
        commands.append(f"({pdf_escape(line)}) Tj")

    commands.append("ET")
    return ("\n".join(commands) + "\n").encode("ascii")


def write_pdf(pages: list[list[str]], output_path: Path) -> None:
    objects: list[bytes] = []

    def add_object(body: bytes) -> int:
        objects.append(body)
        return len(objects)

    font_id = add_object(b"<< /Type /Font /Subtype /Type1 /BaseFont /Helvetica >>")
    page_ids: list[int] = []
    content_ids: list[int] = []

    for page_lines in pages:
        stream = content_stream(page_lines)
        content_id = add_object(
            b"<< /Length " + str(len(stream)).encode("ascii") + b" >>\nstream\n"
            + stream
            + b"endstream"
        )
        content_ids.append(content_id)
        page_ids.append(0)

    pages_id = len(objects) + len(page_ids) + 1

    for index, _ in enumerate(page_ids):
        body = (
            f"<< /Type /Page /Parent {pages_id} 0 R "
            f"/MediaBox [0 0 {PAGE_WIDTH} {PAGE_HEIGHT}] "
            f"/Resources << /Font << /F1 {font_id} 0 R >> >> "
            f"/Contents {content_ids[index]} 0 R >>"
        ).encode("ascii")
        page_ids[index] = add_object(body)

    kids = " ".join(f"{page_id} 0 R" for page_id in page_ids)
    pages_id = add_object(f"<< /Type /Pages /Kids [{kids}] /Count {len(page_ids)} >>".encode("ascii"))
    catalog_id = add_object(f"<< /Type /Catalog /Pages {pages_id} 0 R >>".encode("ascii"))

    pdf = bytearray(b"%PDF-1.4\n")
    offsets = [0]

    for object_id, body in enumerate(objects, start=1):
        offsets.append(len(pdf))
        pdf.extend(f"{object_id} 0 obj\n".encode("ascii"))
        pdf.extend(body)
        pdf.extend(b"\nendobj\n")

    xref_offset = len(pdf)
    pdf.extend(f"xref\n0 {len(objects) + 1}\n".encode("ascii"))
    pdf.extend(b"0000000000 65535 f \n")

    for offset in offsets[1:]:
        pdf.extend(f"{offset:010d} 00000 n \n".encode("ascii"))

    pdf.extend(
        (
            f"trailer\n<< /Size {len(objects) + 1} /Root {catalog_id} 0 R >>\n"
            f"startxref\n{xref_offset}\n%%EOF\n"
        ).encode("ascii")
    )

    output_path.write_bytes(pdf)


def main() -> None:
    root = Path(__file__).resolve().parents[1]
    markdown_path = root / "documentation" / "portfolio.md"
    output_path = root / "documentation" / "portfolio.pdf"

    lines = markdown_to_lines(markdown_path.read_text(encoding="utf-8"))
    write_pdf(paginate(lines), output_path)
    print(f"Wrote {output_path}")


if __name__ == "__main__":
    main()
