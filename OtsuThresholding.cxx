#include <itkOtsuThresholdCalculator.h>
#include <itkImageToHistogramFilter.h>
#include <itkImage.h>
#include <itkImageFileReader.h>
#include <itkOtsuMultipleThresholdsCalculator.h>
#include <itkImageFileWriter.h>
#include <itkMinimumMaximumImageFilter.h>
#include <itkBinaryThresholdImageFilter.h>
#include <sstream>

#define NB 3

int main(int argc , char* argv[] )
{
  if( argc < 2 )
  {
    std::cout<<"usage: "<<argv[0]<<" input_image"<<std::endl;
    return -1 ;
  }
  typedef itk::Image< float , 3 > ImageType ;
  typedef itk::ImageFileReader< ImageType > ReaderType ;
  ReaderType::Pointer reader = ReaderType::New() ;
  reader->SetFileName(argv[1] ) ;
  reader->Update() ;
  typedef itk::Statistics::ImageToHistogramFilter<ImageType> ImageToHistogramType ;
  ImageToHistogramType::Pointer imageToHistogram = ImageToHistogramType::New() ;
  imageToHistogram->SetInput( reader->GetOutput() ) ;
  imageToHistogram->Update() ;

  typedef itk::OtsuMultipleThresholdsCalculator< ImageToHistogramType::HistogramType >
  OtsuThresholdsCalculatorType;

  OtsuThresholdsCalculatorType::Pointer  otsucalculator = OtsuThresholdsCalculatorType::New();
  otsucalculator->SetNumberOfThresholds(NB);
  otsucalculator->SetInputHistogram( imageToHistogram->GetOutput() ) ;
  otsucalculator->Update();
//  float threshold = static_cast<float>( otsucalculator->GetThreshold());
const OtsuThresholdsCalculatorType::OutputType filterThresholds=   otsucalculator->GetOutput();
  for( int i = 0 ; i < filterThresholds.size() ; i++ )
  {
    std::cout << "Otsu threshold "<<i<<" :" << filterThresholds[i] << std::endl;
  }
  typedef itk::MinimumMaximumImageFilter< ImageType > MinMaxFilterType ;
  MinMaxFilterType::Pointer minMax = MinMaxFilterType::New() ;
  minMax->SetInput( reader->GetOutput() ) ;
  minMax->Update() ;
  ImageType::PixelType min = minMax->GetMinimum() ;
  ImageType::PixelType max = minMax->GetMaximum() ;
  std::cout<<"Minimum: "<<min<<std::endl;
  std::cout<<"Maximum: "<<max<<std::endl;
  std::vector< ImageType::PixelType > thresholds ;
  thresholds.push_back( min + .00000001 ) ;
  thresholds.push_back( filterThresholds[ 1 ] ) ;
  thresholds.push_back( filterThresholds[ 2 ] ) ;
/*  for( unsigned int i = 0 ; i < NB ; i++ )
  {
    thresholds.push_back( filterThresholds[ i ] ) ;
  }*/
  thresholds.push_back( max + 1 ) ;
  typedef itk::Image< unsigned char , 3 > MaskType ;
  typedef itk::BinaryThresholdImageFilter<ImageType , MaskType> BinaryFilterType ;
  BinaryFilterType::Pointer filter = BinaryFilterType::New() ;
  filter->SetInput( reader->GetOutput() ) ;
  filter->SetInsideValue( 255 ) ;
  filter->SetOutsideValue( 0 ) ;
  typedef itk::ImageFileWriter< MaskType > WriterType ;
  WriterType::Pointer writer = WriterType::New() ;
  std::ostringstream convert ;
  for( unsigned int i = 0 ; i < thresholds.size() - 1 ; i++ )
  {
    filter->SetLowerThreshold( thresholds[ i ] ) ;
    filter->SetUpperThreshold( thresholds[ i + 1 ] ) ;
    filter->Update() ;
    convert.clear() ;
    convert.str("");
    convert<<i+1 ;
    std::string filename = convert.str() + ".mha" ;
    std::cout<< filename << std::endl ;
    writer->SetFileName( filename.c_str() ) ;
    writer->SetInput( filter->GetOutput() ) ;
    writer->UseCompressionOn() ;
    writer->Update() ;
  }
  
  return 0 ;
}
