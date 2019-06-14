!volume data building

module volume_m
    use ut_m
    use field_m
    use params_m
    use debug_m
    use solver_m
    implicit none
    private
    public :: volume__initialize,  &
              volume__write

!3-D single precision real arrays.
real(SR), dimension(:,:,:), allocatable :: volume_ps !pressure
real(SR), dimension(:,:,:), allocatable :: volume_en !enstrophy

logical, save :: Initialize_done = .false.
integer(SI), parameter :: FILE_VOLUME0 = 21
integer(SI), parameter :: FILE_VOLUME1 = 22
integer(SI), parameter :: FILE_VOLUME2 = 23
integer(SI), parameter :: FILE_VOLUME3 = 24

contains


  subroutine make_single_precision_field(vel,ps)
    type(field__vector3d_t),       intent(in) :: vel
    real(DR), dimension(NX,NY,NZ), intent(in) :: ps

    type(field__vector3d_t)       :: vor   ! vorticity
    real(DR), dimension(NX,NY,NZ) :: enstrophy

    vor = operator_curl(vel)
    enstrophy = operator_dot_product(vor,vor)

    volume_ps = real(       ps(:,:,:),SR)
    volume_en = real(enstrophy(:,:,:),SR)

    call debug__print('called volume/make_single_precision_field.')
    end subroutine make_single_precision_field




!
!Private
!============
!Public
!

    subroutine volume__initialize
    allocate(volume_ps(NX,NY,NZ),   &
             volume_en(NX,NY,NZ))

    call debug__print('volume data allocated')

    open(FILE_VOLUME0,file='PESSURE_DATA1', form='unformatted', access='direct', recl=NX*NY*NZ*4)
    open(FILE_VOLUME1,file='ENSTROPHY_DATA1', form='unformatted', access='direct', recl=NX*NY*NZ*4)
    open(FILE_VOLUME2,file='PESSURE_DATA2', form='unformatted', access='direct', recl=NX*NY*NZ*4)
    open(FILE_VOLUME3,file='ENSTROPHY_DATA2', form='unformatted', access='direct', recl=NX*NY*NZ*4)
    Initialize_done = .true.

    call debug__print('called volume__initlilize')
 end subroutine volume__initialize

subroutine volume__write(nloop,fluid)
    integer(DI),          intent(in) :: nloop
    type(field__fluid_t), intent(in) :: fluid

    type(field__vector3d_t) :: vel
    call solver__get_subfield(fluid,vel)
    call make_single_precision_field(vel,fluid%pressure)
    if (nloop==500) then
     write(FILE_VOLUME0) volume_ps
     write(FILE_VOLUME1) volume_en
    else if (nloop==1000) then
     write(FILE_VOLUME2) volume_ps
     write(FILE_VOLUME3) volume_en
    end if
 end subroutine volume__write

end module volume_m
