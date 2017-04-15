		org	0

direct		rmb	1
bits		rmb	1
index		rmb	32

		org	$100

ext_index	rmb	32
extended	rmb	1

immediate	equ	$cd

		org	$200

		brset	0, bits, *	; $00 thru $0f
		brclr	0, bits, *
		brset	1, bits, *
		brclr	1, bits, *
		brset	2, bits, *
		brclr	2, bits, *
		brset	3, bits, *
		brclr	3, bits, *
		brset	4, bits, *
		brclr	4, bits, *
		brset	5, bits, *
		brclr	5, bits, *
		brset	6, bits, *
		brclr	6, bits, *
		brset	7, bits, *
		brclr	7, bits, *

		bset	0, bits		; $10 thru $1f
		bclr	0, bits
		bset	1, bits
		bclr	1, bits
		bset	2, bits
		bclr	2, bits
		bset	3, bits
		bclr	3, bits
		bset	4, bits
		bclr	4, bits
		bset	5, bits
		bclr	5, bits
		bset	6, bits
		bclr	6, bits
		bset	7, bits
		bclr	7, bits

		bra	*		; $20 thru $2f
		brn	*
		bhi	*
		bls	*
		bcc	*
		bcs	*
		bne	*
		beq	*
		bhcc	*
		bhcs	*
		bpl	*
		bmi	*
		bmc	*
		bms	*
		bil	*
		bih	*

		neg	direct		; $30 thru $3f


		com	direct
		lsr	direct

		ror	direct
		asr	direct
		lsl	direct
		rol	direct
		dec	direct

		inc	direct
		tst	direct

		clr	direct

		nega			; $40 thru $4f

		mul
		coma
		lsra

		rora
		asra
		lsla
		rola
		deca

		inca
		tsta

		clra

		negx			; $50 thru $5f


		comx
		lsrx

		rorx
		asrx
		lslx
		rolx
		decx

		incx
		tstx

		clrx

		neg	index, X	; $60 thru $6f


		com	index, X
		lsr	index, X

		ror	index, X
		asr	index, X
		lsl	index, X
		rol	index, X
		dec	index, X

		inc	index, X
		tst	index, X

		clr	index, X

		neg	, X		; $70 thru $7f


		com	, X
		lsr	, X

		ror	, X
		asr	, X
		lsl	, X
		rol	, X
		dec	, X

		inc	, X
		tst	, X

		clr	, X

		rti			; $80 thru $8f
		rts

		swi










		stop
		wait

					; $90 thru $9f






		tax
		clc
		sec
		cli
		sei
		rsp
		nop

		txa

		sub	#immediate	; $a0 thru $af
		cmp	#immediate
		sbc	#immediate
		cpx	#immediate
		and	#immediate
		bit	#immediate
		lda	#immediate

		eor	#immediate
		adc	#immediate
		ora	#immediate
		add	#immediate

		bsr	*
		ldx	#immediate


		sub	direct		; $b0 thru $bf
		cmp	direct
		sbc	direct
		cpx	direct
		and	direct
		bit	direct
		lda	direct
		sta	direct
		eor	direct
		adc	direct
		ora	direct
		add	direct
		jmp	direct
		jsr	direct
		ldx	direct
		stx	direct

		sub	extended	; $c0 thru $cf
		cmp	extended
		sbc	extended
		cpx	extended
		and	extended
		bit	extended
		lda	extended
		sta	extended
		eor	extended
		adc	extended
		ora	extended
		add	extended
		jmp	extended
		jsr	extended
		ldx	extended
		stx	extended

		sub	ext_index, X	; $d0 thru $df
		cmp	ext_index, X
		sbc	ext_index, X
		cpx	ext_index, X
		and	ext_index, X
		bit	ext_index, X
		lda	ext_index, X
		sta	ext_index, X
		eor	ext_index, X
		adc	ext_index, X
		ora	ext_index, X
		add	ext_index, X
		jmp	ext_index, X
		jsr	ext_index, X
		ldx	ext_index, X
		stx	ext_index, X

		sub	index, X	; $e0 thru $ef
		cmp	index, X
		sbc	index, X
		cpx	index, X
		and	index, X
		bit	index, X
		lda	index, X
		sta	index, X
		eor	index, X
		adc	index, X
		ora	index, X
		add	index, X
		jmp	index, X
		jsr	index, X
		ldx	index, X
		stx	index, X

		sub	, X		; $f0 thru $ff
		cmp	, X
		sbc	, X
		cpx	, X
		and	, X
		bit	, X
		lda	, X
		sta	, X
		eor	, X
		adc	, X
		ora	, X
		add	, X
		jmp	, X
		jsr	, X
		ldx	, X
		stx	, X

		end

  Uncomment out the 