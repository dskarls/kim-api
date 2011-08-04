!-------------------------------------------------------------------------------
!
! setup_B2_KIM_API_object : Create KIM API object, 
!                           allocate memory and set known values.
!
!-------------------------------------------------------------------------------
subroutine setup_B2_KIM_API_object(pkim, testname, modelname, specname1, specname2)
  use KIMservice
  implicit none

  !-- Transferred variables
  integer(kind=kim_intptr), intent(in)  :: pkim
  character(len=80),        intent(in)  :: testname
  character(len=80),        intent(in)  :: modelname
  character(len=2),         intent(in)  :: specname1
  character(len=2),         intent(in)  :: specname2

  !-- Local variables
  integer(kind=kim_intptr) :: N = 2 ! hard-wired to two atoms
  integer, parameter :: ATypes = 2  ! hard-wired to two atomic types
  integer ier, N4
  integer(kind=8) numberOfAtoms; pointer(pnAtoms,numberOfAtoms)
  integer numberAtomTypes;       pointer(pnAtomTypes,numberAtomTypes)
  integer atomTypesdum(1);       pointer(patomTypesdum,atomTypesdum)
  integer, pointer :: atomTypes(:)

  ! Initialize KIM API object
  !
  ier = kim_api_init_f(pkim, testname, modelname)
  if (ier.le.0) then
     call report_error(__LINE__, "kim_api_init_f", ier)
     stop
  endif
  call kim_api_allocate_f(pkim, N, ATypes, ier)
  if (ier.le.0) then
     call report_error(__LINE__, "kim_api_allocate_f", ier)
     stop
  endif

  ! call model's init routine
  !
  ier = kim_api_model_init_f(pkim)
  if (ier.le.0) then
     call report_error(__LINE__, "kim_api_model_init_f", ier)
     stop
  endif

  ! Unpack data from KIM object whose values need to be set
  !
  pnAtoms = kim_api_get_data_f(pkim, "numberOfAtoms", ier);
  if (ier.le.0) then
     call report_error(__LINE__, "kim_api_get_data_f", ier)
     stop
  endif

  pnAtomTypes = kim_api_get_data_f(pkim, "numberAtomTypes", ier)
  if (ier.le.0) then
     call report_error(__LINE__, "kim_api_get_data_f", ier)
     stop
  endif

  patomTypesdum = kim_api_get_data_f(pkim, "atomTypes", ier)
  if (ier.le.0) then
     call report_error(__LINE__, "kim_api_get_data_f", ier)
     stop
  endif
  N4 = N
  call toIntegerArrayWithDescriptor1d(atomTypesdum, atomTypes, N4)

  ! Set values
  !
  numberOfAtoms   = N
  numberAtomTypes = ATypes
  atomTypes(1)    = kim_api_get_atypecode_f(pkim, specname1, ier)
  if (ier.le.0) then
     call report_error(__LINE__, "kim_api_get_atypecode_f", ier)
     stop
  endif
  atomTypes(2)    = kim_api_get_atypecode_f(pkim, specname2, ier)
  if (ier.le.0) then
     call report_error(__LINE__, "kim_api_get_atypecode_f", ier)
     stop
  endif

  return

end subroutine setup_B2_KIM_API_object