/*======================================================================

  This file is part of the elastix software.

  Copyright (c) University Medical Center Utrecht. All rights reserved.
  See src/CopyrightElastix.txt or http://elastix.isi.uu.nl/legal.php for
  details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE. See the above copyright notices for more information.

======================================================================*/

#ifndef __elxAdvancedAffineTransform_H_
#define __elxAdvancedAffineTransform_H_

#include "itkAdvancedMatrixOffsetTransformBase.h"
#include "itkAdvancedCombinationTransform.h"

#include "itkCenteredTransformInitializer.h"
#include "elxIncludes.h"

namespace elastix
{
using namespace itk;

  /**
   * \class AdvancedAffineTransformElastix
   * \brief A transform based on the itk::AdvancedMatrixOffsetTransformBase.
   *
   * This transform is an affine transformation.
   *
   * The first couple of parameters (4 in 2D and 9 in 3D) define the affine
   * matrix, the last couple (2 in 2D and 3 in 3D) define the translation.
   *
   * The parameters used in this class are:
   * \parameter Transform: Select this transform as follows:\n
   *    <tt>(%Transform "AffineTransform")</tt>
   * \parameter Scales: the scale factor between the rotations and translations,
   *    used in the optimizer. \n
   *    example: <tt>(Scales 200000.0)</tt> \n
   *    example: <tt>(Scales 100000.0 60000.0 ... 80000.0)</tt> \n
   *    If only one argument is given, that factor is used for the rotations.
   *    If more than one argument is given, then the number of arguments should be
   *    equal to the number of parameters: for each parameter its scale factor.
   *    If this parameter option is not used, by default the rotations are scaled
   *    by a factor of 100000.0. See also the AutomaticScalesEstimation parameter.
   * \parameter AutomaticScalesEstimation: if this parameter is set to "true" the Scales
   *    parameter is ignored and the scales are determined automatically. \n
   *    example: <tt>( AutomaticScalesEstimation "true" ) </tt> \n
   *    Default: "false" (for backwards compatibility). Recommended: "true".
   * \parameter CenterOfRotation: an index around which the image is rotated. \n
   *    example: <tt>(CenterOfRotation 128 128 90)</tt> \n
   *    By default the CenterOfRotation is set to the geometric center of the image.
   * \parameter AutomaticTransformInitialization: whether or not the initial translation
   *    between images should be estimated as the distance between their centers.\n
   *    example: <tt>(AutomaticTransformInitialization "true")</tt> \n
   *    By default "false" is assumed. So, no initial translation.\n
   *    
   *
   * The transform parameters necessary for transformix, additionally defined by this class, are:
   * \transformparameter CenterOfRotation: stores the center of rotation as an index. \n
   *    example: <tt>(CenterOfRotation 128 128 90)</tt> 
   *    deprecated! From elastix version 3.402 this is changed to CenterOfRotationPoint!
   * \transformparameter CenterOfRotationPoint: stores the center of rotation, expressed in world coordinates. \n
   *    example: <tt>(CenterOfRotationPoint 10.555 6.666 12.345)</tt>
   *
   * \ingroup Transforms
   */
  
  template < class TElastix >
    class AdvancedAffineTransformElastix :
      public AdvancedCombinationTransform<
          ITK_TYPENAME elx::TransformBase<TElastix>::CoordRepType,
          elx::TransformBase<TElastix>::FixedImageDimension > ,
      public elx::TransformBase<TElastix>
  {
  public:

    /** Standard ITK-stuff. */
    typedef AdvancedAffineTransformElastix                    Self;
    typedef AdvancedCombinationTransform<
      typename elx::TransformBase<TElastix>::CoordRepType,
      elx::TransformBase<TElastix>::FixedImageDimension >     Superclass1;
    typedef elx::TransformBase<TElastix>                      Superclass2;
    typedef SmartPointer<Self>                                Pointer;
    typedef SmartPointer<const Self>                          ConstPointer;

    /** The ITK-class that provides most of the functionality, and
     * that is set as the "CurrentTransform" in the CombinationTransform */
    typedef AdvancedMatrixOffsetTransformBase<
      typename elx::TransformBase<TElastix>::CoordRepType,
      elx::TransformBase<TElastix>::FixedImageDimension,
      elx::TransformBase<TElastix>::MovingImageDimension >     AffineTransformType;
    
    /** Method for creation through the object factory. */
    itkNewMacro( Self );
    
    /** Run-time type information (and related methods). */
    itkTypeMacro( AdvancedAffineTransformElastix, AdvancedCombinationTransform );

    /** Name of this class.
     * Use this name in the parameter file to select this specific transform. \n
     * example: <tt>(Transform "AffineTransform")</tt>\n
     */
    elxClassNameMacro( "AffineTransform" );
    
    /** Dimension of the domain space. */
    itkStaticConstMacro( SpaceDimension, unsigned int, Superclass2::FixedImageDimension );
    
    /** Typedefs inherited from the superclass. */
    typedef typename Superclass1::ScalarType                ScalarType;
    typedef typename Superclass1::ParametersType            ParametersType;
    typedef typename Superclass1::JacobianType              JacobianType;
    typedef typename Superclass1::InputVectorType           InputVectorType;
    typedef typename Superclass1::OutputVectorType          OutputVectorType;
    typedef typename Superclass1::InputCovariantVectorType  InputCovariantVectorType;
    typedef typename Superclass1::OutputCovariantVectorType OutputCovariantVectorType;
    typedef typename Superclass1::InputVnlVectorType        InputVnlVectorType;
    typedef typename Superclass1::OutputVnlVectorType       OutputVnlVectorType;
    typedef typename Superclass1::InputPointType            InputPointType;
    typedef typename Superclass1::OutputPointType           OutputPointType;
    
    /** Typedef's from the TransformBase class. */
    typedef typename Superclass2::ElastixType               ElastixType;
    typedef typename Superclass2::ElastixPointer            ElastixPointer;
    typedef typename Superclass2::ConfigurationType         ConfigurationType;
    typedef typename Superclass2::ConfigurationPointer      ConfigurationPointer;
    typedef typename Superclass2::RegistrationType          RegistrationType;
    typedef typename Superclass2::RegistrationPointer       RegistrationPointer;
    typedef typename Superclass2::CoordRepType              CoordRepType;
    typedef typename Superclass2::FixedImageType            FixedImageType;
    typedef typename Superclass2::MovingImageType           MovingImageType;
    typedef typename Superclass2::ITKBaseType               ITKBaseType;
    typedef typename Superclass2::CombinationTransformType  CombinationTransformType;
    
    /** Other typedef's. */
    typedef typename FixedImageType::IndexType              IndexType;
    typedef typename IndexType::IndexValueType              IndexValueType;
    typedef typename FixedImageType::SizeType               SizeType;
    typedef typename FixedImageType::PointType              PointType;
    typedef typename FixedImageType::SpacingType            SpacingType;
    typedef typename FixedImageType::RegionType             RegionType;
    typedef typename FixedImageType::DirectionType          DirectionType;
    
    typedef CenteredTransformInitializer<
      AffineTransformType, FixedImageType, MovingImageType> TransformInitializerType;
    typedef typename TransformInitializerType::Pointer      TransformInitializerPointer;
    typedef typename AffineTransformType::Pointer           AffineTransformPointer;

    /** For scales setting in the optimizer */
    typedef typename Superclass2::ScalesType                ScalesType;
    
    /** Execute stuff before the actual registration:
     * \li Call InitializeTransform
     * \li Set the scales.
     */
    virtual void BeforeRegistration( void );
    
    /** Initialize Transform.
     * \li Set all parameters to zero. 
     * \li Set center of rotation:
     *   automatically initialized to the geometric center of the image, or
     *   assigned a user entered voxel index, given by the parameter 
     *   (CenterOfRotation <index-x> <index-y> ...). 
     *   If an initial transform is present and HowToCombineTransforms is
     *   set to "Compose", the initial transform is taken into account 
     *   while setting the center of rotation.
     * \li Set initial translation:
     *  the initial translation between fixed and moving image is guessed,
     *  if the user has set (AutomaticTransformInitialization "true"). 
     * 
     * It is not yet possible to enter an initial rotation angle or scaling.
     */
    virtual void InitializeTransform( void );
    
    /** Set the scales
     * \li If AutomaticScalesEstimation is "true" estimate scales
     * \li If scales are provided by the user use those,
     * \li Otherwise use some default value
     * This function is called by BeforeRegistration, after
     * the InitializeTransform function is called 
     */
    virtual void SetScales( void );
    
    /** Function to read transform-parameters from a file. 
     * 
     * It reads the center of rotation and calls the superclass' implementation.
     */
    virtual void ReadFromFile( void );
    
    /** Function to write transform-parameters to a file. 
     * It writes the center of rotation to file and calls the superclass' implementation.
     */
    virtual void WriteToFile( const ParametersType & param ) const;

  protected:

    /** The constructor. */
    AdvancedAffineTransformElastix();
    /** The destructor. */
    virtual ~AdvancedAffineTransformElastix() {};

    /** Try to read the CenterOfRotation from the transform parameter file
     * This is an index value, and, thus, converted to world coordinates.
     * Transform parameter files generated by elastix version < 3.402
     * saved the center of rotation in this way.
     */
    virtual bool ReadCenterOfRotationIndex( InputPointType & rotationPoint ) const;

    /** Try to read the CenterOfRotationPoint from the transform parameter file 
     * The CenterOfRotationPoint is already in world coordinates.
     * Transform parameter files generated by elastix version > 3.402
     * save the center of rotation in this way.
     */
    virtual bool ReadCenterOfRotationPoint( InputPointType & rotationPoint ) const;
    
  private:

    /** The private constructor. */
    AdvancedAffineTransformElastix( const Self& );  // purposely not implemented
    /** The private copy constructor. */
    void operator=( const Self& );          // purposely not implemented

    AffineTransformPointer m_AffineTransform;
    
  }; // end class AdvancedAffineTransformElastix
  
  
} // end namespace elastix

#ifndef ITK_MANUAL_INSTANTIATION
#include "elxAdvancedAffineTransform.hxx"
#endif

#endif // end #ifndef __elxAdvancedAffineTransform_H_
