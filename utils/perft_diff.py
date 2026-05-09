import sys

def parse_perft(filename):
    data = {}
    try:
        with open(filename, 'r') as f:
            for line in f:
                if ':' in line:
                    parts = line.strip().split(':')
                    move = parts[0].strip()
                    count = int(parts[1].strip())
                    data[move] = count
    except FileNotFoundError:
        print(f"Error: {filename} not found.")
        sys.exit(1)
    return data

def compare_perft(file_a, file_b):
    data_a = parse_perft(file_a)
    data_b = parse_perft(file_b)

    # Get all unique moves from both files
    all_moves = sorted(set(data_a.keys()) | set(data_b.keys()))
    
    # Dynamic headers based on filenames
    header_a = file_a
    header_b = file_b
    
    print(f"{'Move':<10} | {header_a:<15} | {header_b:<15} | {'Difference'}")
    print("-" * 65)

    differences = 0
    for move in all_moves:
        val_a = data_a.get(move, "MISSING")
        val_b = data_b.get(move, "MISSING")

        if val_a != val_b:
            if isinstance(val_a, int) and isinstance(val_b, int):
                diff = val_b - val_a
                # Add a '+' sign for positive differences
                diff_str = f"{diff:+}" 
            else:
                diff_str = "N/A"
                
            print(f"{move:<10} | {str(val_a):<15} | {str(val_b):<15} | {diff_str}")
            differences += 1

    print("-" * 65)
    if differences == 0:
        print("Success: No differences found!")
    else:
        print(f"Summary: Found {differences} moves with different node counts.")

if __name__ == "__main__":
    if len(sys.argv) < 3:
        print("Usage: python3 perft_diff.py <file1.txt> <file2.txt>")
    else:
        compare_perft(sys.argv[1], sys.argv[2])