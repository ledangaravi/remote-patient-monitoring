
/*Library functions from standard C drivers by STMicrolectronics (lsm6dsl_reg.cpp, h -- https://github.com/STMicroelectronics/STMems_Standard_C_drivers). 
High-level functions for pedometer inspired by mbed example (https://os.mbed.com/teams/ST/code/Pedometer_IKS01A2/) */



#define BDU 0x40
#define odrmode 0b0010110


/*set output data rate, must be at 26Hz for pedometer functionality*/
int set_x_odr_when_enabled (float odr){ 

	lsm6dsl_odr_xl_t new_odr;	  
  
	new_odr = ( odr <=   13.0f ) ? LSM6DSL_XL_ODR_12Hz5
		  : ( odr <=   26.0f ) ? LSM6DSL_XL_ODR_26Hz
		  : ( odr <=   52.0f ) ? LSM6DSL_XL_ODR_52Hz
		  : ( odr <=  104.0f ) ? LSM6DSL_XL_ODR_104Hz
		  : ( odr <=  208.0f ) ? LSM6DSL_XL_ODR_208Hz
		  : ( odr <=  416.0f ) ? LSM6DSL_XL_ODR_416Hz
		  : ( odr <=  833.0f ) ? LSM6DSL_XL_ODR_833Hz
		  : ( odr <= 1660.0f ) ? LSM6DSL_XL_ODR_1k66Hz
		  : ( odr <= 3330.0f ) ? LSM6DSL_XL_ODR_3k33Hz
		  :                      LSM6DSL_XL_ODR_6k66Hz;
            
  if ( lsm6dsl_xl_data_rate_set(&dev_ctx, new_odr ) == error ) 
  {   
   return 1;
  }
  
  
  return 0;
}

int set_x_odr_when_disabled (float odr)
{ 
  _x_last_odr = ( odr <=   13.0f ) ? 13.0f
             : ( odr <=   26.0f ) ? 26.0f
             : ( odr <=   52.0f ) ? 52.0f
             : ( odr <=  104.0f ) ? 104.0f
             : ( odr <=  208.0f ) ? 208.0f
             : ( odr <=  416.0f ) ? 416.0f
             : ( odr <=  833.0f ) ? 833.0f
             : ( odr <= 1660.0f ) ? 1660.0f
             : ( odr <= 3330.0f ) ? 3330.0f
             :                      6660.0f;
                                 
  return 0;
}

int set_x_odr(float odr)
{

  if(_x_is_enabled == 1)
  {
    if(set_x_odr_when_enabled(odr) == 1) 
    {
      return 1;
    }
  }
  else
  {
    if(set_x_odr_when_disabled(odr) == 1) 
    {
      return 1;
    }
  }
  
  return 0;
}
 

/*set accelerometer scale*/
int set_x_fs(float fullScale)
{

  lsm6dsl_fs_xl_t new_fs;
  
  new_fs = ( fullScale <= 2.0f ) ? LSM6DSL_2g
         : ( fullScale <= 4.0f ) ? LSM6DSL_4g
         : ( fullScale <= 8.0f ) ? LSM6DSL_8g
         :                         LSM6DSL_16g;
           
  if ( lsm6dsl_xl_full_scale_set( &dev_ctx, new_fs ) == error)
  {
    return 1;
  }
  
  return 0;
}




/*enable accelerometer*/
int enable_x(void) {

   if ( _x_is_enabled == 1 )
  {
    return 0;
  }
  
  // Output data rate selection. 
  if ( set_x_odr_when_enabled( _x_last_odr ) == 1 )
  {
    return 1;
  }
  
  lsm6dsl_xl_filter_analog_set(&dev_ctx, LSM6DSL_XL_ANA_BW_400Hz); //in-built DSP filtering
  
  lsm6dsl_xl_lp2_bandwidth_set(&dev_ctx, LSM6DSL_XL_LOW_NOISE_LP_ODR_DIV_100); //in-built DSP filtering LPF

  _x_is_enabled = 1;
  
  return 0;	
}

/*define threshold for pedometer positive 'step'*/
int set_pedometer_threshold(uint8_t ped_thresh)
{

  if (lsm6dsl_pedo_threshold_set( &dev_ctx, ped_thresh) == error)
  {
    return 1;
  }
  
  return 0;
}


int enable_pedometer(void)
{

  // Output Data Rate selection 
  if( set_x_odr(26.0f) == error )
  {
    return 1;
  }
  
  // Full scale selection. 
  if( set_x_fs(2.0f) == error )
  {
    return 1;
  }
  
  // Set pedometer threshold. 
  if (set_pedometer_threshold(LSM6DSL_PEDOMETER_THRESHOLD_MID_LOW) == 1)
  {
    return 1;
  }


    if ( lsm6dsl_pedo_sens_set(&dev_ctx, 1) == error )
  {
    return 1;
  }

  step.int1_step_detector = 1;
  if(lsm6dsl_pin_int1_route_set(&dev_ctx, &step) == error) //set pedometer to signal interrupt
  {
	return 1;
  }
  
  return 0;
}

/*raw x,y,z values for motion artefact reduction*/
int32_t lsm6dsl_acceleration_raw_get1(lsm6dsl_ctx_t *ctx, uint8_t *buff)
{
  int32_t ret;
  uint8_t i, j, k;
  uint8_t bytes_per_axis = 2;

  k=0;
  for (i=0;i<3;i++)
  {
    for (j=0;j<bytes_per_axis;j++)
    {
        if(!lsm6dsl_read_reg(ctx, LSM6DSL_OUTX_L_XL + k, &buff[k], 1))
          return MEMS_ERROR;
        k++;
    }

  }
  return 0; 
 }

/*read values from FIFO storage*/
 int32_t lsm6dsl_read_fifo(lsm6dsl_ctx_t *ctx, uint8_t *buff)
{
  int32_t ret;
  uint8_t i;

  uint8_t reg[2];

  if(lsm6dsl_read_reg(ctx, LSM6DSL_FIFO_STATUS1, reg, 1) == MEMS_ERROR)
          return MEMS_ERROR;

  if(lsm6dsl_read_reg(ctx, LSM6DSL_FIFO_STATUS2, &reg[1], 1) == MEMS_ERROR)
          return MEMS_ERROR;

  uint16_t fifo_level = ((reg[1] << 8) & (0x0F)) | reg[0];

  for (i=0;i<fifo_level;i++)
  {
        if(lsm6dsl_read_reg(ctx, LSM6DSL_FIFO_DATA_OUT_L, &buff[i*2], 2) == MEMS_ERROR)
          return MEMS_ERROR;
  }
  return 0; // SUCCESS
 }

/*process raw values to suitable formatting based on sensitivity chosen (2g)*/
int get_x_axes_raw(float_t *pData)
{
  

  uint8_t regValue[6] = {0, 0, 0, 0, 0, 0};


  // Read output registers from LSM6DSL_ACC_GYRO_OUTX_L_XL to LSM6DSL_ACC_GYRO_OUTZ_H_XL. 
  if ( lsm6dsl_read_fifo( &dev_ctx, regValue) == MEMS_ERROR )
  {
    return 1;
  }

  // Format the data. 
  pData[0] = lsm6dsl_from_fs2g_to_mg(( ( ( ( int16_t )regValue[1] ) << 8 ) + ( int16_t )regValue[0] ));
  pData[1] = lsm6dsl_from_fs2g_to_mg(( ( ( ( int16_t )regValue[3] ) << 8 ) + ( int16_t )regValue[2] ));
  pData[2] = lsm6dsl_from_fs2g_to_mg(( ( ( ( int16_t )regValue[5] ) << 8 ) + ( int16_t )regValue[4] ));

  return 0;
}

/*initialise continuous mode FIFO, data rate, and enable required registers for processing*/
int lsm6dsl_fifo_init(){
uint8_t conf_pl[] = {0x01};
uint8_t odr[] = {odrmode};

lsm6dsl_block_data_update_set(&dev_ctx, PROPERTY_ENABLE);


lsm6dsl_fifo_mode_set(&dev_ctx, LSM6DSL_STREAM_MODE); //continuous mode
lsm6dsl_write_reg(&dev_ctx, LSM6DSL_FIFO_CTRL5, odr, 1);


lsm6dsl_fifo_data_rate_set(&dev_ctx, LSM6DSL_FIFO_26Hz); //26Hz settting


lsm6dsl_write_reg(&dev_ctx, LSM6DSL_FIFO_CTRL3, conf_pl, 1);

return 0;
}