import glob, subprocess, sys

ANSWERS = {
    'tests/test1.txt': "Yes\nYes\nYes\nNo\nNo",
    'tests/test2.txt': "Yes\nYes\nYes\nYes\nNo\nNo",
    'tests/test3.txt': "Yes\nNo\nNo",
    'tests/test4.txt': "Yes\nYes\nYes\nYes\nNo",
    'tests/test5.txt': "Yes\nNo",
    'tests/test6.txt': "Yes\nYes\nYes\nNo\nYes",
    'tests/test7.txt': "No\nNo",
    'tests/test8.txt': "Yes\nNo"
}

failed = False

for t in sorted(glob.glob('tests/*.txt')):
    print(f"Test {t} ", end=" ")
    with open(t, 'r') as f:
        res = subprocess.run(['./earley'], stdin=f, capture_output=True, text=True)
    actual = res.stdout.strip()
    expected = ANSWERS.get(t, "").strip()
    if not expected:
        print(f"SKIPPED (No answer key)\nOutput:\n{actual}")
        continue
    if actual == expected:
        print("OK")
    else:
        print("FAIL")
        print(f"--- EXPECTED ---\n{expected}")
        print(f"--- GOT ---\n{actual}")
        print("----------------")
        failed = True
if failed:
    sys.exit(1)