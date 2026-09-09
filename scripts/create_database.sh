#!/usr/bin/env bash

set -euo pipefail

script_dir="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
project_dir="$(cd -- "${script_dir}/.." && pwd)"
schema_path="${project_dir}/database/schema.sql"
database_path="${1:-${project_dir}/data/energy_prices.sqlite3}"

if ! command -v sqlite3 >/dev/null 2>&1; then
    echo "Error: sqlite3 is not installed." >&2
    exit 1
fi

mkdir -p -- "$(dirname -- "${database_path}")"

sqlite3 -bail "${database_path}" < "${schema_path}"

echo "Database initialized: ${database_path}"
