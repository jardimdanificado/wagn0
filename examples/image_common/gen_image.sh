#!/usr/bin/env bash
# gen_image.sh — generate test PNG and animated GIF, convert to C arrays.
# Usage: ./gen_image.sh <output_dir>
#   output_dir: where to write test.png, test.gif, png_data.h, gif_data.h
# Requires: ImageMagick (convert), xxd

set -euo pipefail

OUT_DIR="${1:-.}"

# Create test PNG: 64x64 gradient with shapes
convert -size 64x64 \
    gradient:'#ff5577-#3357ff' \
    -draw "fill yellow circle 32,32 32,8" \
    -draw "fill black rectangle 8,8 24,24" \
    -draw "fill white text 0,60 'PNG'" \
    "$OUT_DIR/test.png"

# Create animated GIF: 3 frames, 100ms each
convert -size 64x64 xc:'#ff0000' \
    -fill yellow -draw "circle 32,32 32,8" \
    "$OUT_DIR/_frame1.png"
convert -size 64x64 xc:'#00ff00' \
    -fill blue -draw "rectangle 16,16 48,48" \
    "$OUT_DIR/_frame2.png"
convert -size 64x64 xc:'#0000ff' \
    -fill white -draw "circle 32,32 32,8" \
    -draw "fill black text 8,36 'W'" \
    "$OUT_DIR/_frame3.png"

convert -delay 100 -loop 0 \
    "$OUT_DIR/_frame1.png" \
    "$OUT_DIR/_frame2.png" \
    "$OUT_DIR/_frame3.png" \
    "$OUT_DIR/test.gif"

rm -f "$OUT_DIR/_frame1.png" "$OUT_DIR/_frame2.png" "$OUT_DIR/_frame3.png"

PNG_SIZE=$(stat -c%s "$OUT_DIR/test.png")
GIF_SIZE=$(stat -c%s "$OUT_DIR/test.gif")

xxd -i "$OUT_DIR/test.png" \
    | sed -e 's/^unsigned char/const unsigned char/' \
          -e 's/__test_png/png_data/' \
          -e 's/png_data_len/png_size/' \
    > "$OUT_DIR/png_data.h"

xxd -i "$OUT_DIR/test.gif" \
    | sed -e 's/^unsigned char/const unsigned char/' \
          -e 's/__test_gif/gif_data/' \
          -e 's/gif_data_len/gif_size/' \
    > "$OUT_DIR/gif_data.h"

echo "Generated:"
echo "  test.png: $PNG_SIZE bytes"
echo "  test.gif: $GIF_SIZE bytes"
echo "  png_data.h, gif_data.h in $OUT_DIR"
