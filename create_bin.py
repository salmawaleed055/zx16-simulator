binary_data = bytes.fromhex(
    "76be fca1 7c66 0061 00b9 0443 008e c0a1 1443 38b9 2443 06b9 3443 786e 0061 22b9 "
    "00ce 00e1 0443 0241 06c1 c0da 0443 0241 06c1 c0da 7c66 0061 22b9 00ce 00e1 0443 0241 "
    "06c1 c0f0 00ce 00e1 0443 0241 06c1 c0f0 ffff"
)
with open("tests_new/graphics_test_new.bin", "wb") as f:
    f.write(binary_data)
