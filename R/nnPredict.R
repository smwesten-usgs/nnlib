#' nnpredict - Perform a single forward run of the neural network model.
#'
#' @param model_name Name of the model that defines the neural network in iQuest format.
#' @param input_vec Numeric vector with the same length as the number of input nodes.
#' @param echo Toggle printing of debug statements within neural network C code.
#'
#' @return Numeric vector of neural network model outputs corresponding to the inputs provided.
#' @export
#' @useDynLib nnlib nninfo nnpredict
#'
#' @examples
#' model_filename   <- file.path(system.file("extdata/WI_StreamTemp.enn", package="nnlib") )
#' static_filename  <- file.path(system.file("extdata/static_data.csv", package="nnlib") )
#' dynamic_filename <- file.path(system.file("extdata/dynamic_data.csv", package="nnlib") )
#' static_data  <- read.csv( static_filename, header=TRUE )
#' dynamic_data <- read.csv( dynamic_filename, header=TRUE )
#'
#' # select a row from each of the static and dynamic data files
#' input_vector <- as.numeric( c( static_data[ 37, 3:10], dynamic_data[ 3, c(2,3,4,5,6,7,10,16,18)] ) )
#'
#' # now run the forward neural network model with a single days' worth of data
#' stream_temp  <- nn_predict( model_name=model_filename, input_vec=input_vector )
#'
nn_predict <- function(model_name, input_vec, echo=FALSE) {

  input_vec <- as.numeric(input_vec)

  echo_int <- ifelse(echo,1,0)

  num_in <- .C("nninfo",model_name,nin=integer(1),
               nout=integer(1),
               nhid=integer(1) )$nin

  num_out <- .C("nninfo",model_name,nin=integer(1),
                nout=integer(1),
                nhid=integer(1) )$nout

  if (length(input_vec) != num_in) {
    stop(paste("Number of values in your input vector (", as.character(length(input_vec)),
               ") does not match the number specified in the model (",as.character(num_in),").", sep=""))
  }

  output <- numeric( as.integer(num_out) )
  output_vec <- numeric( as.integer( num_out ) )

  # make actual neural network prediction
  output_vec <- .C("nnpredict",as.character(model_name),
                   as.numeric(input_vec),
                   output=numeric(num_out),
                   as.integer(echo_int) )$output

  return( as.numeric(output_vec) )

}

