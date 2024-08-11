# How to use

This package is highly paraller, you can always try to use more CPUs to make it more efficient

## 1. Data download use Noisepy
     In Download folder, you could S0A_dowload_ASDF_MPI.ipynb (Jupyter notebook) to Download data from IRIS (HD5); If you have your own data in sac, please refer to Noisepy about data format transformation.
## 2. Data preprocessing
     In Download folder, please use S0C_ASDF_MPI_Processing.ipynb to do rma, whitening data preprocessing
## 3. Compile the code
    In main folder, type scons to make the necessary files
## 4.  Data format transformation 
     ./run1_retrive_data.bash extract daily noise data into RSF format, which is input data for later similarity calculation. I will try to keep using .h5 format in the future
## 5.  Prepare station pairs: sh run2_get_station_pairs.bash
     Sometimes, we do not need CCFs that are very close to virtual sources (offset > 1-2 wavelenth)
## 6.  Get global CCF: sbatch run2_get_gcorr.bash.
     There are several parameters:
     nccfs=1440 (number of local CCFs used in next step)
     taumax=120 (maximum time lag in second)
     gcorr_len=3600 (global correlation time, 1 hour)
     gcorr_overlap=2700 (overlap, 0.75 hour)
## 7.  Get local CCFs: run2_get_lcorr.bash
      There are several parameters:
      num_of_lccfs=1440 (In theory, we have CCFs on every time samples, that is 24*3600 CCFs for one day data. In order to improve the efficiency, we donwsample it to 1440, which is every 60 time steps).
      taumax=120
      gaussian_window=1800 (similar to global correlation time window. The Gaussian window is used to windowing the noise data to get local correlations)
    
## 8.  Obtaining global CCFs
     Stacking the global CCFs and expand it into 2D data, which have same shape as local CCFs: sbatch run3_get_reference.bash
## 9.  Get local similarity
     sbatch run4_simi.bash get local similaties
## 10. Get local stacking
     sbatch run5_lstack.bash get stacked local/global CCFs; If you want to get running mean sqaure root ration based selective stacking, then sbatch run5_sstack.bash
## 11. Get sac data
     sbatch run6_sum_lfold_to_sac.bash reformat RSF results into SAC for later analysis

