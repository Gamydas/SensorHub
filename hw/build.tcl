### GLOBALS ###
set project_name "sensorhub"
set script_folder [file dirname [file normalize [info script]]]
set language "VHDL"
# tcl scripts are located in <rep_root>/hw/
set repo_root [file dirname $script_folder] 
#### IMPORTANT DO NOT CHANGE ####
set board "digilentinc.com:arty-z7-10:part0:1.1"
set chip "xc7z010clg400-1"

#### IMPORTANT DO NOT CHANGE ####



################################################################
# Check if script is running in correct Vivado version.
################################################################
set scripts_vivado_version 2026.1
set current_vivado_version [version -short]

if { [string first $scripts_vivado_version $current_vivado_version] == -1 } {
   puts ""
   if { [string compare $scripts_vivado_version $current_vivado_version] > 0 } {
      catch {common::send_gid_msg -ssname BD::TCL -id 2042 -severity "ERROR" "This script expects Vivado Version <$scripts_vivado_version>, but is being run in <$current_vivado_version> of Vivado. Sourcing the script failed since it was written for a future version of Vivado."}

   } else {
     catch {common::send_gid_msg -ssname BD::TCL -id 2041 -severity "ERROR" "This script expects Vivado Version  <$scripts_vivado_version>, but is being run in <$current_vivado_version> of Vivado. Please run the script in Vivado <$scripts_vivado_version> then open the design in Vivado <$current_vivado_version>."}

   }

   error "Version Error"
}

set_param board.repoPaths [file join $::env(HOME) .Xilinx Vivado $scripts_vivado_version xhub board_store xilinx_board_store]

### Creates build directory and new .xpr project file
create_project $project_name [file join $repo_root build] -part $chip -force 
set_property BOARD_PART $board [current_project]
set_property target_language $language [current_project]
