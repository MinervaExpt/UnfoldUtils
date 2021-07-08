import os
import sys



base_dir_prefix = os.path.join(os.getenv("HOME"),"data","UnfoldingStudies","MigrationMatrix")
variations = ["_CV","_piontune","_2p2h","_rpa","_default","_rpapiontune","_2p2hrpa"]
#variatons = ["_CV","rpa"]
#_infile = "Migration_MakeFlux-1_ApplyFlux-1_Multiplicity-0_CombinedPlaylists-me1A.root"
#_infile = "Merged_MigrationMatrix_optimized_minervame5A_6A_6B.root"
#target_dir = "/minerva/data/users/bashyal8/UnfoldingStudies/pzpt_unfolding/"
#target_dir = "/minerva/data/users/"+os.getenv("USER")+"/UnfoldingStudies"
target_dir = os.path.join(os.getenv("HOME"),"data","UnfoldingStudies_hms")
reco_hist = "h_q2_enu_qelike_reco"  #"h_pzmu_ptmu_qelike_reco"
truth_hist = "h_q2_enu_qelike_truth"#"h_pzmu_ptmu_qelike_truth"
migration_hist = "h_q2_enu_qelike_migration"#"h_pzmu_ptmu_qelike_migration"
_output_prefix = reco_hist.replace("qelike_reco","")
output_prefix = _output_prefix.replace("h","stat_universe")
#variation = variations[2]
_infile = "MigrationMatrix_enu.root"
num_dim = str(2)
num_uni = str(25)
num_iter = "1,2,3,4,5,6,10"
#num_iter = "20"
#num_iter = "1,3,4,5,10"
max_chi2 = str(1000)
step_chi2 = str(10)


#needed hists
##########
# --data --> Data to be unfolded
# --data_truth -->True Data Distribution
# --reco -->Reconstructed MC distribution
# --truth -->Truth MC distribution
# --migration -->Migration matrix for Reconstructed MC to True MC
#########

for variation in variations:
  output_file = target_dir+"/"+output_prefix+variation+"_"+num_uni+".root"
  dat_file = base_dir_prefix+variation+"/"+_infile
  mc_file = base_dir_prefix+variations[0]+"/"+_infile #should be 0 mostly....
  mc_file = base_dir_prefix+variations[0]+"/"+_infile #should be 0 mostly....
  migration_file = mc_file #I think migration matrix is only constructed from MC info...

  cmd = "../TransWarpExtraction -o "+output_file\
    + " --data_file "+dat_file \
    + " --data "+reco_hist \
    + " --data_truth_file "+dat_file \
    + " --data_truth "+truth_hist \
    + " --reco_file "+mc_file \
    + " --reco "+reco_hist \
    + " --truth_file "+mc_file \
    + " --truth "+truth_hist \
    + " --migration_file "+migration_file \
    + " --migration "+migration_hist \
    + " --num_dim "+num_dim \
    + " --num_uni "+num_uni \
    + " --num_iter "+num_iter \
    + " --max_chi2 "+max_chi2 \
    + " --step_chi2 "+step_chi2 
#    + " --exclude_bin "

  print cmd

  os.system(cmd)
