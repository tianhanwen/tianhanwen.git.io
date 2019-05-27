# -*- coding: utf-8 -*-
"""
Created on Wed Mar 28 14:41:04 2018

@author: shinyonsei2d
"""

''' 
The order of LF image files may be different with this file.
(Top to Bottom, Left to Right, and so on..)
  
If you use different LF images, 

you should change our 'func_makeinput.py' file.

# Light field images: input_Cam000-080.png
# All viewpoints = 9x9(81)

# -- LF viewpoint ordering --
# 00 01 02 03 04 05 06 07 08
# 09 10 11 12 13 14 15 16 17
# 18 19 20 21 22 23 24 25 26
# 27 28 29 30 31 32 33 34 35
# 36 37 38 39 40 41 42 43 44
# 45 46 47 48 49 50 51 52 53
# 54 55 56 57 58 59 60 61 62
# 63 64 65 66 67 68 69 70 71
# 72 73 74 75 76 77 78 79 80
 

# We use star-shape 9x9 viewpoints 
# for depth estimation
#
# 00          04          08
#    10       13       16 
#       20    22    24 
#          30 31 32 
# 36 37 38 39 40 41 42 43 44
#          48 49 50 
#       56    58    60 
#    64       67       70 
# 72          76          80    
f
'''

#import numpy as np
import numpy as np
import os
import time
from epinet_fun.func_pfm import write_pfm
from epinet_fun.func_makeinput import make_epiinput
from epinet_fun.func_makeinput import make_multiinput
from epinet_fun.func_epinetmodel_3Dconv import layer1_multistream
from epinet_fun.func_epinetmodel_3Dconv import layer2_merged
from epinet_fun.func_epinetmodel_3Dconv import layer3_last
from epinet_fun.func_epinetmodel_3Dconv import define_epinet3D

import matplotlib.pyplot as plt
import imageio


#这是把以前2D卷积核的网络输入，转换为
def transpose3D(fourDim):
    fiveDim = fourDim[:,:,:,:,np.newaxis]
    fiveDim = fiveDim.transpose(0,3,2,1,4)
    fiveDim = fiveDim.transpose(0,1,3,2,4)
    return fiveDim
if __name__ == '__main__':
    
    # Input : input_Cam000-080.png
    # Depth output : image_name.pfm
    dir_output='epinet_output/out3D'
    dir_output512 = 'epinet_output/out3D_512'
    if not os.path.exists(dir_output):
        os.makedirs(dir_output)   

    # GPU setting ( gtx 1080ti - gpu0 ) 
    os.environ["CUDA_DEVICE_ORDER"]="PCI_BUS_ID" #按照PCI_BUS_ID顺序从0开始排列GPU设备
    os.environ["CUDA_VISIBLE_DEVICES"]="0" #设置当前使用的GPU设备仅为0号设备 设备名称为 ‘/gpu:0’

    '''
    /// Setting 1. LF Images Directory
    
    Setting01_LFdir = 'synthetic': Test synthetic LF images (from 4D Light Field Benchmark)
                                 "A Dataset and Evaluation Methodology for 
                                 Depth Estimation on 4D Light Fields".
                                http://hci-lightfield.iwr.uni-heidelberg.de/

    Setting01_LFdir = 'Lytro': Test real LF images(Lytro)
    
    '''
    Setting01_LFdir = 'synthetic'
#    Setting01_LFdir='Lytro'
    
    if(Setting01_LFdir=='synthetic'):    
        dir_LFimages=['hci_dataset/test/bedroom','hci_dataset/test/bicycle'
                     ,'hci_dataset/test/herbs','hci_dataset/test/origami']
        #dir_LFimages = ['hci_dataset/stratified/backgammon', 'hci_dataset/stratified/dots'
        #   , 'hci_dataset/stratified/pyramids', 'hci_dataset/stratified/stripes']
        #dir_LFimages=['hci_dataset/training/boxes', 'hci_dataset/training/sideboard'
        #             ,'hci_dataset/training/dino']
        #dir_LFimages = ['hci_dataset/additional/antinous', 'hci_dataset/additional/museum',
        #                'hci_dataset/additional/boardgames','hci_dataset/additional/dishes'
         #               ,'hci_dataset/additional/greek','hci_dataset/additional/kitchen'
         #               ,'hci_dataset/additional/medieval2','hci_dataset/additional/pens'
         #              ,'hci_dataset/additional/pillows','hci_dataset/additional/platonic'
          #              ,'hci_dataset/additional/rosemary','hci_dataset/additional/table'
          #              ,'hci_dataset/additional/tomb','hci_dataset/additional/tower'
         #               ,'hci_dataset/additional/town','hci_dataset/additional/vinyl']
        image_w=512
        image_h=512
        
    elif(Setting01_LFdir=='Lytro'): 
        dir_LFimages=['lytro/2067']    
        image_w=552
        image_h=383  
        
        
        
        
    '''
    /// Setting 2. Angular Views 
    
    Setting02_AngualrViews = [2,3,4,5,6] : 5x5 viewpoints
    
    Setting02_AngualrViews = [0,1,2,3,4,5,6,7,8] : 9x9 viewpoints
    
    # ------ 5x5 viewpoints -----
    #                                  
    #       20    22    24 
    #          30 31 32 
    #       38 39 40 41 42      
    #          48 49 50 
    #       56    58    60 
    #                         
    # ---------------------------                      
    
    # ------ 9x9 viewpoints -----
    # 
    # 00          04          08
    #    10       13       16 
    #       20    22    24 
    #          30 31 32 
    # 36 37 38 39 40 41 42 43 44
    #          48 49 50 
    #       56    58    60 
    #    64       67       70 
    # 72          76          80       
    #
    # ---------------------------
    '''
    
#    Setting02_AngualrViews = [2,3,4,5,6]  # number of views ( 2~6 for 5x5 )     
    Setting02_AngualrViews = [0,1,2,3,4,5,6,7,8]  # number of views ( 0~8 for 9x9 ) 

    if(len(Setting02_AngualrViews)==5):
        path_weight='epinet_checkpoints/pretrained_5x5.hdf5' # sample weight.    
    if(len(Setting02_AngualrViews)==9):
        #path_weight = 'epinet_checkpoints/EPINET_train_ckp/iter0240_trainmse1.879_bp7.78.hdf5'
        path_weight='epinet_checkpoints/model3D/iter0002_trainmse8.748_bp48.87.hdf5' # sample weight
        #path_weight='E:/bsdm/epinet-master/epinet_checkpoints/pretrained_9x9.hdf5'
        #path_weight='epinet_checkpoints/EPINET_train_ckp/iter0211_trainmse1.754_bp8.33.hdf5'




    img_scale=1 #   1 for small_baseline(default) <3.5px, 
                # 0.5 for large_baseline images   <  7px
                
    img_scale_inv=int(1/img_scale)
    
    

    ''' Define Model ( set parameters )'''
    
    model_conv_depth=6
    model_filt_num=70
    model_learning_rate=0.1**5
    model_512=define_epinet3D(round(img_scale*image_h),
                            round(img_scale*image_w),
                            Setting02_AngualrViews,
                            model_conv_depth, 
                            model_filt_num,
                            model_learning_rate)
    


    ''' Model Initialization '''
    
    model_512.load_weights(path_weight)
    dum_sz=model_512.input_shape[0]
    #dum=np.zeros((1,dum_sz[1],dum_sz[2],dum_sz[3]),dtype=np.float32)
    #dummy=model_512.predict([dum,dum, dum,dum],batch_size=1)
    #以上两行由2d改为一下3d的输入，以前的输入是[1,512,512,9],现在的输入是[1,9,512,512,1]
    dum = np.zeros((1,dum_sz[1],dum_sz[2],dum_sz[3],1),dtype=np.float32)
    dummy = model_512.predict([dum,dum, dum,dum],batch_size=1)
    hight = 512
    width = 512
    pfm512 = np.zeros((512,512),dtype=np.float32)

    
    
    
    """  Depth Estimation  """
    for image_path in dir_LFimages:


        (val_90d , val_0d, val_45d, val_M45d)=make_multiinput(image_path,
                                                             image_h,
                                                             image_w,
                                                             Setting02_AngualrViews)
        #转换为3维输入
        val_90d  = transpose3D(val_90d)
        val_0d = transpose3D(val_0d)
        val_45d = transpose3D(val_45d)
        val_M45d = transpose3D(val_M45d)
        start=time.clock()
        # predict
        #val_output_tmp=model_512.predict([ val_90d[:,::img_scale_inv,::img_scale_inv],
        #                                    val_0d[:,::img_scale_inv,::img_scale_inv],
        #                                  val_45d[:,::img_scale_inv,::img_scale_inv],
        #                                 val_M45d[:,::img_scale_inv,::img_scale_inv]],
        #                                  batch_size=1);
        val_output_tmp=model_512.predict([ val_90d,
                                            val_0d,
                                          val_45d,
                                        val_M45d],
                                          batch_size=1);


        runtime=time.clock() - start
        print("begin predict")
        plt.imshow(val_output_tmp[0,:,:,0])
        plt.imsave(dir_output+'/%s.png' % (image_path.split('/')[-1]),val_output_tmp[0,:,:,0])
        pfm512[11:501,11:501] = val_output_tmp[0,:,:,0] #为了给像素补充O,便于比较结果
        plt.imsave(dir_output512+'/%s.png' % (image_path.split('/')[-1]),pfm512) #存成另一个结果

        plt.show()
        print("runtime: %.5f(s)" % runtime)
         
        # save .pfm file
        write_pfm(val_output_tmp[0,:,:,0], dir_output+'/%s.pfm' % (image_path.split('/')[-1]))

        write_pfm(pfm512,dir_output512+'/%s.pfm' % (image_path.split('/')[-1]))#存成另一个结果
        print('pfm file saved in %s/%s.pfm' % (dir_output,image_path.split('/')[-1]))


