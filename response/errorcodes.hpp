/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   errorcodes.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nschumac <nschumac@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/09 14:34:23 by nschumac          #+#    #+#             */
/*   Updated: 2022/03/09 14:42:13 by nschumac         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

typedef enum t_responsecodes
{
	CONTINUE = 100,
	SWITCHING_PROTOCOLS,
	EARLY_HINTS = 103,

	OK = 200,
	CREATED,
	ACCEPTED,
	NON_AUTHORITATIVE_INFORMATION,
	NO_CONTENT,
	RESET_CONTENT,
	PARTIAL_CONTENT,

	MULTIPLE_CHOICES = 300,
	MOVED_PERMAMENTLY,
	FOUND,
	SEE_OTHER,
	NOT_MODIFIED,
	TEMPORARY_REDIRECT,
	PERMANENT_REDIRECT,

	BAD_REQUEST = 400,
	UNAUTHORIZED,
	PAYMENT_REQUIRED,
	FORBIDDEN,
	NOT_FOUND,
	METHOD_NOT_ALLOWED,
	NOT_ACCEPTABLE,
	PROXY_AUTHENTICATION_REQUIRED,
	REQUEST_TIMEOUT,
	CONFLICT,
	GONE,
	LENGTH_REQUIRED,
	PRECONDITION_FAILED,
	PAYLOAD_TOO_LARGE,
	URI_TOO_LONG,
	UNSUPPORTED_MEDIA_TYPE,
	RANGE_NOT_STISFIABLE,
	EXPECTATION_FAILED,
	IM_A_TEAPOT,
	UNPROCESSABLE_ENTITY,
	TOO_EARLY,
	UPGRADE_REQUIRED,
	PRECONDITION_REQUIRED,
	TOO_MANY_REQUESTS,
	REQUEST_HEADER_FIELDS_TOO_LARGE,
	UNAVAILABLE_FOR_LEGAL_REASONS,

	INTERNAL_SERVER_ERROR = 500,
	NOT_IMPLEMENTED,
	BAD_GATEWAY,
	SERVICE_UNAVAILABLE,
	GATEWAY_TIMEOUT,
	HTTP_VERSION_NOT_SUPPORTED,
	VARIANT_ALSO_NEGOTIATES,
	INSUFFICIENT_STORAGE,
	LOOP_DETECTED,
	NOT_EXTENDED,
	NETWORK_AUTHENTICATION_REQUIRED
} responsecodes;
