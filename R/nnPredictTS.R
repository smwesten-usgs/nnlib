#' nn_predict_ts - Perform a series of neural network predictions for a set of dynamic model inputs.
#'
#' @param model_name Name of the model that defines the neural network in iQuest format.
#' @param num_static_fields Number of static neural network inputs to be processed.
#' @param num_dyn_fields Number of dynamic neural network inputs to be processed.
#' @param input_static_vec Vector of static neural network model intputs.
#' @param input_dyn_vec Vector of dynamic neural network model inputs; length will be equal to the number
#'  of dynamic fields times the number of repetitions (i.e. days) worth of data included in the vector.
#' @param echo Print debug information within the neural network C code if TRUE.
#' @param field_order Dynamic inputs can be rearranged according to the field_order vector.
#'
#' @return Matrix of output values; columns represent values of the output nodes of the ANN. Number of rows
#'         will equal the length of the input dynamic vector (input_dyn_vec).
#' @export
#' @examples
#' model_filename   <- file.path(system.file("extdata/WI_StreamTemp.enn", package="nnlib") )
#' static_filename  <- file.path(system.file("extdata/static_data.csv", package="nnlib") )
#' dynamic_filename <- file.path(system.file("extdata/dynamic_data.csv", package="nnlib") )
#' static_data  <- read.csv( static_filename, header=TRUE )
#' dynamic_data <- read.csv( dynamic_filename, header=TRUE )
#'
#' # select a row from each of the static and dynamic data files
#' static_input_vector  <- as.numeric( static_data[ 37, 3:10] )
#' dynamic_input_matrix <- dynamic_data[ 3:7, c( 2, 3, 4, 5, 6, 7, 8, 13, 15) ]
#' dynamic_input_vector <- as.vector( t( dynamic_input_matrix ) )
#'
#' # now run the forward neural network model with multiple days' worth of data
#' stream_temps  <- nn_predict_ts( model_name=model_filename,
#'                                 num_static_fields=8,
#'                                 num_dyn_fields=9,
#'                                 input_static_vec=static_input_vector,
#'                                 input_dyn_vec=dynamic_input_vector )
#'
nn_predict_ts <- function(model_name, num_static_fields, num_dyn_fields,
                          input_static_vec, input_dyn_vec, echo=FALSE, field_order=NA) {

# get number of input nodes
num_out <- .C("nninfo",model_name, integer(1),
                       nout=integer(1),
                       integer(1) )$nout

num_in <- .C("nninfo",model_name, nin=integer(1),
                      integer(1),
                      integer(1) )$nin

num_inputs <- num_static_fields + num_dyn_fields

if ( any( is.na( field_order ) ) ) {
  field_order <- c(0:(num_inputs - 1))
} else {
  field_order <- field_order - 1
}

num_dyn_recs <- length( input_dyn_vec ) / num_dyn_fields
num_outputs <- num_dyn_recs * num_out

if ( num_inputs != num_in ) {
  stop("Number of input fields does not equal the number of input nodes of the ANN.")
}

if ( length( field_order ) != num_in ) {
  stop("Length of field_order vector must be equal to the number of ANN model inputs.")
}

if ( min( field_order ) != 0 ) {
  stop("The minimum value in the field_order vector must be 1.")
}

if ( max( field_order ) != ( num_inputs - 1) ) {
  stop("The maximum value in the field_order vector must be equal to the number of ANN model inputs.")
}

# make prediction given the trained ANN and input
out_vec <- .C("nnpredictTS",
          model_name,
          as.integer(num_dyn_fields),
          as.integer(num_dyn_recs),
          as.numeric(input_dyn_vec),
          as.integer(num_static_fields),
          as.numeric(input_static_vec),
          output=numeric(num_outputs),
          as.integer(echo),
          as.integer(field_order) )$output

out_matrix <- matrix(data=out_vec, ncol=num_out, byrow=TRUE)

return(out_matrix)
}

