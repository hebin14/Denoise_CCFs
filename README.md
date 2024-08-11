# How to Use

This package is highly parallelized; you can always increase the number of CPUs to improve efficiency.

## 1. Data Download Using Noisepy
In the `Download` folder, use the `S0A_dowload_ASDF_MPI.ipynb` (Jupyter notebook) to download data from IRIS (in HDF5 format). If you have your own data in SAC format, refer to Noisepy for instructions on data format transformation.

## 2. Data Preprocessing
In the `Download` folder, use `S0C_ASDF_MPI_Processing.ipynb` for RMA and whitening data preprocessing.

## 3. Compile the Code
In the `main` folder, type `scons` to compile the necessary files.

## 4. Data Format Transformation
Run `./run1_retrieve_data.bash` to extract daily noise data into RSF format, which will be used as input data for later similarity calculations. Future updates will aim to maintain the .h5 format.

## 5. Prepare Station Pairs
Use `sh run2_get_station_pairs.bash` to prepare station pairs. Note that CCFs very close to virtual sources (offset > 1-2 wavelengths) may not be needed.

## 6. Get Global CCF
Run `sbatch run2_get_gcorr.bash` to generate global CCF. Key parameters include:
- `nccfs=1440` (number of local CCFs used in the next step)
- `taumax=120` (maximum time lag in seconds)
- `gcorr_len=3600` (global correlation time, 1 hour)
- `gcorr_overlap=2700` (overlap, 0.75 hours)

## 7. Get Local CCFs
Run `run2_get_lcorr.bash` to generate local CCFs. Key parameters include:
- `num_of_lccfs=1440` (downsampling to improve efficiency, originally 24*3600 CCFs per day)
- `taumax=120`
- `gaussian_window=1800` (similar to the global correlation time window, used for windowing noise data to obtain local correlations)

## 8. Obtain Global CCFs
Stack the global CCFs and expand them into 2D data, matching the shape of local CCFs: `sbatch run3_get_reference.bash`.

## 9. Get Local Similarity
Run `sbatch run4_simi.bash` to obtain local similarities.

## 10. Get Local Stacking
Run `sbatch run5_lstack.bash` to generate stacked local/global CCFs. For running mean square root ratio-based selective stacking, use `sbatch run5_sstack.bash`.

## 11. Get SAC Data
Run `sbatch run6_sum_lfold_to_sac.bash` to reformat RSF results into SAC format for later analysis.
