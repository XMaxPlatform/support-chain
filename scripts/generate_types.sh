VERSION=1.0
ulimit -u

source _xmax_evm.sh

gen_exe="${build_path}/foundation/basetypes/types_generator/types_generator"

input_file="${xmax_path}/foundation/basetypes/types_generator/types.xmax"

output_file="${xmax_path}/foundation/basetypes/include/gentypes.hpp"

"./${gen_exe}" "${input_file}" "${output_file}"

read -n 1 -p "Press any key to continue..."
